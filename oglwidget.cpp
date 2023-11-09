#include "oglwidget.h"

// Vertex data for the cube
static const float vertices[] = {
    // Positions         // Colors
    // Front face
    -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
    1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
    1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
    1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,

    // Back face
    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
    1.0f,  1.0f, -1.0f,  0.5f, 0.5f, 0.5f,
    1.0f,  1.0f, -1.0f,  0.5f, 0.5f, 0.5f,
    -1.0f,  1.0f, -1.0f,  0.5f, 1.0f, 0.5f,
    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,

    // Left face
    -1.0f,  1.0f,  1.0f,  1.0f, 0.5f, 0.0f,
    -1.0f,  1.0f, -1.0f,  0.5f, 0.0f, 0.5f,
    -1.0f, -1.0f, -1.0f,  0.0f, 0.5f, 1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 0.5f, 1.0f,
    -1.0f, -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,  1.0f, 0.5f, 0.0f,

    // Right face
    1.0f,  1.0f,  1.0f,  0.5f, 1.0f, 1.0f,
    1.0f,  1.0f, -1.0f,  0.5f, 0.5f, 0.5f,
    1.0f, -1.0f, -1.0f,  1.0f, 0.5f, 0.5f,
    1.0f, -1.0f, -1.0f,  1.0f, 0.5f, 0.5f,
    1.0f, -1.0f,  1.0f,  0.5f, 1.0f, 1.0f,
    1.0f,  1.0f,  1.0f,  0.5f, 1.0f, 1.0f,

    // Bottom face
    -1.0f, -1.0f, -1.0f,  0.0f, 0.5f, 0.0f,
    1.0f, -1.0f, -1.0f,  0.5f, 0.5f, 1.0f,
    1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.5f,
    1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.5f,
    -1.0f, -1.0f,  1.0f,  0.5f, 1.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 0.5f, 0.0f,

    // Top face
    -1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
    1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.5f,
    1.0f,  1.0f,  1.0f,  0.5f, 0.5f, 0.5f,
    1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f
};

OGLWidget::OGLWidget(QWidget *parent) : QOpenGLWidget(parent), m_vbo(QOpenGLBuffer::VertexBuffer)
{

}

OGLWidget::~OGLWidget()
{
    makeCurrent();

    m_vbo.destroy();
    m_vao.destroy();
    delete m_program;

    doneCurrent();
}

void OGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Setup shaders
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "PointCloudVertexShader.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "PointCloudFragmentShader.frag");
    m_program->link();

    // Setup VAO and VBO
    //m_vao.create();
    //m_vao.bind();

    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(vertices, sizeof(vertices));

    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(float) * 6);
    m_program->enableAttributeArray(1);
    m_program->setAttributeBuffer(1, GL_FLOAT, sizeof(float) * 3, 3, sizeof(float) * 6);

    // Set projection matrix
    m_proj.perspective(90.0f, 4.0f/3.0f, 0.1f, 1000.0f);
    m_camera.lookAt(QVector3D(1,0,5), QVector3D(0,0,0), QVector3D(0,1,0));
}

void OGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void OGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();
    //m_vao.bind();

    m_camera.rotate(0.5f, 0, 1, 0);

    m_program->setUniformValue("projMatrix", m_proj);
    m_program->setUniformValue("viewMatrix", m_camera);
    m_program->setUniformValue("modelMatrix", m_world);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    update();
}
