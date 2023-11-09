#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>

#include <QMouseEvent>
#include <QVector3D>
#include <QMatrix4x4>
#include <QOpenGLShader>

#include <vector>

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    // constructors/destructors
    Renderer(QWidget *parent = nullptr);
    ~Renderer();

    // public functions
    //// OpenGL functions
    virtual void initializeGL() override = 0;
    virtual void resizeGL(int width, int height) override = 0;
    virtual void paintGL() override = 0;

protected:
    // protected functions
    //// camera movement
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    //// visualization tools
    void showGrid();
    void populateGrid(int gridSize, float gridSpacing,  float setGridColor[3]);
    void showCordsSystem();
    void populateCordsSystem(float axisLength, float setCordsColor[3]);
    void showTrajectory();
    void populateTrajectory();
    void showTools();
    void populateTools();

    // protected variables
    //// OpenGL variables
    QMatrix4x4 transformMatrix;
    QMatrix4x4 modelMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projectionMatrix;

private:
    // private functions
    //// common functions
    void initVariables();

    //// tools functions
    ////// grid
    void GenerateGridVertices(int gridSize, float gridSpacing);

    ////// cords system
    std::vector<GLfloat> generateCoordinateSystemVertices(float axis_length);

    // private variables
    //// camera variables
    QPoint lastMousePosition;

    QVector3D position;
    QVector3D forward;
    QVector3D up;
    QVector3D right;

    float moveSpeed;
    float rotationSpeed;
    float zoomSpeed;

    //// tools variables
    ////// grid
    GLuint gridVAO;
    GLuint gridVBO;
    QOpenGLShaderProgram *gridShaderProgram;
    std::vector<GLfloat> gridVertices;
    GLfloat gridColor[3];

    ////// cords system
    GLuint cordsVAO;
    GLuint cordsVBO;
    QOpenGLShaderProgram *cordsShaderProgram;
    std::vector<GLfloat> cordsVertices;
    GLfloat cordsColor[3];

    ////// trajectory
    GLuint trajectoryVAO;
    GLuint trajectoryVBO;
    QOpenGLShaderProgram *trajectoryShaderProgram;
    std::vector<GLfloat> trajectoryVertices;
    GLfloat trajectoryColor[3];
};

#endif // RENDERER_H
