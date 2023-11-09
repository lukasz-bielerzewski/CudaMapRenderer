#ifndef ST_POINTCLOUDRENDERER_H
#define ST_POINTCLOUDRENDERER_H

#include "renderer.h"
#include "pointcloud.h"

class ST_PointCloudRenderer : public Renderer
{
public:
    // constructors/destructors
    ST_PointCloudRenderer(QWidget *parent);
    ~ST_PointCloudRenderer();

    // public functions
    //// setter functions
    void setData();

protected:
    // protected functions
    //// OpenGL functions
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    // private functions
    //// init functions
    void initVariables();

    // private variables
    //// Point Cloud data
    InputData inputData;
    PointCloud *pointCloud;

    //// OpenGL variables
    float pointsData[];
};

#endif // ST_POINTCLOUDRENDERER_H
