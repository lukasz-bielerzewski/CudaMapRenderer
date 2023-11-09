#include "st_pointcloudrenderer.h"

// constructors/destructors
ST_PointCloudRenderer::ST_PointCloudRenderer(QWidget *parent)
    : Renderer(parent)
{
    this->initVariables();

    this->pointCloud = new PointCloud(this->inputData);
}

ST_PointCloudRenderer::~ST_PointCloudRenderer()
{
    delete this->pointCloud;
}

// public functions
//// setter functions
void ST_PointCloudRenderer::setData()
{
    this->inputData.pathToImagesDirectory = "/home/rezzec/build-CUDA_Map_Renderer-Desktop_Qt_6_5_1_GCC_64bit-Debug/office_kt0";
    this->inputData.pathToTrajectoryFile = "/home/rezzec/build-CUDA_Map_Renderer-Desktop_Qt_6_5_1_GCC_64bit-Debug/office_kt0/traj0.txt";
    this->inputData.pathToAssociationFile = "/home/rezzec/build-CUDA_Map_Renderer-Desktop_Qt_6_5_1_GCC_64bit-Debug/office_kt0/associations.txt";
    this->inputData.maxIndex = 1508;
}

// protected functions
//// OpenGL functions
void ST_PointCloudRenderer::initializeGL()
{
    initializeOpenGLFunctions();
}

void ST_PointCloudRenderer::resizeGL(int w, int h)
{

}

void ST_PointCloudRenderer::paintGL()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// private functions
//// init functions
void ST_PointCloudRenderer::initVariables()
{
    this->inputData.pathToImagesDirectory = "";
    this->inputData.pathToTrajectoryFile = "";
    this->inputData.pathToAssociationFile = "";
    this->inputData.maxIndex = 0;
}
