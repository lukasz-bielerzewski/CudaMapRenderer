#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <opencv2/opencv.hpp>

#include <eigen3/Eigen/Dense>

#include <string>
#include <fstream>
#include <map>

struct TrajectoryData
{
    int id;
    float cam_x, cam_y, cam_z;             // Position
    float qx, qy, qz, qw;      // Orientation (quaternion)
};

struct AssociationData
{
    std::map<int , std::string> rgbData;
    std::map<int , std::string> depthData;
};

struct InputData
{
    std::string pathToImagesDirectory;
    std::string pathToTrajectoryFile;
    std::string pathToAssociationFile;
    unsigned int maxIndex;
};

class PointCloud
{
public:
    // constructors/destructors
    PointCloud(InputData input_data);
    ~PointCloud();

    // public functions
    //// getters
    std::vector<float> getPointsData();

    //// loop function, can either use all images or just selected few passed in string as indexes
    void iterateThroughImages(bool imagesAll = true, int selectedIndexes[] = {} , size_t arraySize = 0);

private:
    // private functions
    //// init functions
    void initializeVariables(InputData &input_data);

    void loadRGBImage(const std::string &path_to_image);
    void loadDepthImage(const std::string &path_to_image);
    void loadTrajectoryData(const std::string &path_to_trajectory);
    void loadAssociationsFile(const std::string &path_to_associations);
    void loadResources();

    //// data transformations
    void transformToPointCloudData(size_t index);

    // private variables
    //// images data
    cv::Mat *rgbImage;
    cv::Mat *depthImage;

    int imageWidth;
    int imageHeight;

    //// imported data
    std::vector<TrajectoryData> *trajectoryData;
    AssociationData *associationData;
    InputData *inputData;

    //// exported data
    std::vector<float> *pointsData;

    float cx, cy, focal_x, focal_y;
};

#endif // POINTCLOUD_H
