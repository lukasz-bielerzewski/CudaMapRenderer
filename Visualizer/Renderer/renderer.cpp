#include "renderer.h"

// constructors/destructors
Renderer::Renderer(QWidget *parent)
{
    this->initVariables();
}

Renderer::~Renderer()
{
    delete this->gridShaderProgram;
    delete this->cordsShaderProgram;
    delete this->trajectoryShaderProgram;
}

// protected functions
//// camera movement
void Renderer::mousePressEvent(QMouseEvent *event)
{
    this->lastMousePosition = event->pos();
}

void Renderer::mouseMoveEvent(QMouseEvent *event)
{
    QPoint delta = event->pos() - this->lastMousePosition;

    if (event->buttons() & Qt::LeftButton) {
        // Translate camera
        this->position += this->right * delta.x() * this->moveSpeed;
        this->position += this->up * delta.y() * this->moveSpeed;

        this->transformMatrix = this->transformMatrix + QMatrix4x4(1.f, 0.f, 0.f, this->position[0],
                                                                   0.f, 1.f, 0.f, this->position[1],
                                                                   0.f, 0.f, 1.f, this->position[2],
                                                                   0.f, 0.f, 0.f, 1.f);

    } else if (event->buttons() & Qt::RightButton) {
        // Rotate camera
        float yaw = delta.x() * this->rotationSpeed;
        float pitch = delta.y() * this->rotationSpeed;

        // Create a rotation matrix for yaw (around the up vector)
        QMatrix4x4 rotationYaw;
        rotationYaw.rotate(yaw, this->up);

        // Create a rotation matrix for pitch (around the right vector)
        QMatrix4x4 rotationPitch;
        rotationPitch.rotate(pitch, this->right);

        // Combine the yaw and pitch rotations
        QMatrix4x4 combinedRotation = rotationYaw * rotationPitch;

        // Apply rotation to the transform matrix
        this->transformMatrix = combinedRotation * this->transformMatrix;

        // Update camera vectors
        this->forward = (rotationYaw * rotationPitch).map(this->forward);
        this->right = QVector3D::crossProduct(this->forward, this->up);
        this->up = QVector3D::crossProduct(this->right, this->forward).normalized();
    }

    this->lastMousePosition = event->pos();
}

void Renderer::wheelEvent(QWheelEvent *event)
{
    int delta = event->angleDelta().y();
    this->position += this->forward * this->zoomSpeed * delta;
}

//// visualization tools
void Renderer::showGrid()
{
    // Use the shader program
    glUseProgram(this->gridShaderProgram->programId());

    // Bind the VAO
    glBindVertexArray(this->gridVAO);

    // Get the location of the uniform variable in the fragment shader
    GLint colorUniformLocation = glGetUniformLocation(this->gridShaderProgram->programId(), "color");
    GLint modelMatrixUniformLocation = glGetUniformLocation(this->gridShaderProgram->programId(), "model");
    GLint viewMatrixUniformLocation = glGetUniformLocation(this->gridShaderProgram->programId(), "view");
    GLint projectionMatrixUniformLocation = glGetUniformLocation(this->gridShaderProgram->programId(), "projection");

    // Set the uniform values
    glUniform3fv(colorUniformLocation, 1, this->gridColor);
    glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, this->modelMatrix.data());
    glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, this->viewMatrix.data());
    glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, this->projectionMatrix.data());

    // Draw
    glDrawArrays(GL_LINES, 0, this->gridVertices.size());

    // Unbind the VAO
    glBindVertexArray(0);

    // release shader program
    glUseProgram(0);
}

void Renderer::populateGrid(int gridSize, float gridSpacing, float setGridColor[3])
{
    // generate grid vertices data
    this->GenerateGridVertices(gridSize, gridSpacing);

    // set grid color
    this->gridColor[0] = setGridColor[0];
    this->gridColor[1] = setGridColor[1];
    this->gridColor[2] = setGridColor[2];

    // setup shader program
    this->gridShaderProgram = new QOpenGLShaderProgram();
    this->gridShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "Visualizer/Shaders/GridVertexShader.vert");
    this->gridShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "Visualizer/Shaders/GridFragmentShader.frag");

    // Generate VAO
    glGenVertexArrays(1, &this->gridVAO);
    glBindVertexArray(this->gridVAO);

    // Generate VBO
    glGenBuffers(1, &this->gridVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->gridVBO);
    glBufferData(GL_ARRAY_BUFFER, this->gridVertices.size(), this->gridVertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO
    glBindVertexArray(0);

}

void Renderer::showCordsSystem()
{

}

void Renderer::populateCordsSystem()
{
    // setup shader program
    this->cordsShaderProgram = new QOpenGLShaderProgram();
    this->cordsShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "Visualizer/Shaders/CordsVertexShader.vert");
    this->cordsShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "Visualizer/Shaders/CordsFragmentShader.frag");
}

void Renderer::showTrajectory()
{

}

void Renderer::populateTrajectory()
{
    // setup shader program
    this->trajectoryShaderProgram = new QOpenGLShaderProgram();
    this->trajectoryShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "Visualizer/Shaders/TrajectoryVertexShader.vert");
    this->trajectoryShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "Visualizer/Shaders/TrajectoryFragmentShader.frag");
}

void Renderer::showTools()
{

}

void Renderer::populateTools()
{

}

// private functions
//// common functions
void Renderer::initVariables()
{
    this->lastMousePosition = {0, 0};
    this->position = {0.f, 0.f, 0.f};
    this->forward = {0.f, 0.f, 0.f};
    this->up = {0.f, 0.f, 0.f};
    this->right = {0.f, 0.f, 0.f};
    this->moveSpeed = 0.f;
    this->rotationSpeed = 0.f;
    this->zoomSpeed = 0.f;
    this->transformMatrix = { 1.f, 0.f, 0.f, 0.f,
                              0.f, 1.f, 0.f, 0.f,
                              0.f, 0.f, 1.f, 0.f,
                             0.f, 0.f, 0.f, 1.f};
}

//// tools functions
////// grid
void Renderer::GenerateGridVertices(int gridSize, float gridSpacing)
{
    this->gridVertices.clear();  // Clear the vector of any previous vertices

    // The total number of lines for each axis is gridSize + 1 (for the grid lines at the edges)
    int lineCount = gridSize + 1;

    // Generate lines along the X-axis
    for (int y = 0; y < lineCount; ++y) {
        for (int z = 0; z < lineCount; ++z) {
            float yPos = y * gridSpacing;
            float zPos = z * gridSpacing;
            // Line start
            this->gridVertices.push_back(0.0f);
            this->gridVertices.push_back(yPos);
            this->gridVertices.push_back(zPos);
            // Line end
            this->gridVertices.push_back(gridSize * gridSpacing);
            this->gridVertices.push_back(yPos);
            this->gridVertices.push_back(zPos);
        }
    }

    // Generate lines along the Y-axis
    for (int x = 0; x < lineCount; ++x) {
        for (int z = 0; z < lineCount; ++z) {
            float xPos = x * gridSpacing;
            float zPos = z * gridSpacing;
            // Line start
            this->gridVertices.push_back(xPos);
            this->gridVertices.push_back(0.0f);
            this->gridVertices.push_back(zPos);
            // Line end
            this->gridVertices.push_back(xPos);
            this->gridVertices.push_back(gridSize * gridSpacing);
            this->gridVertices.push_back(zPos);
        }
    }

    // Generate lines along the Z-axis
    for (int x = 0; x < lineCount; ++x) {
        for (int y = 0; y < lineCount; ++y) {
            float xPos = x * gridSpacing;
            float yPos = y * gridSpacing;
            // Line start
            this->gridVertices.push_back(xPos);
            this->gridVertices.push_back(yPos);
            this->gridVertices.push_back(0.0f);
            // Line end
            this->gridVertices.push_back(xPos);
            this->gridVertices.push_back(yPos);
            this->gridVertices.push_back(gridSize * gridSpacing);
        }
    }
}
