#include "pointcloud.h"

// constructors/destructors
PointCloud::PointCloud(InputData input_data)
{
    this->initializeVariables(input_data);
    this->loadResources();
}

PointCloud::~PointCloud()
{
    delete this->trajectoryData;
    delete this->associationData;
    delete this->inputData;
    delete this->pointsData;
    delete this->rgbImage;
    delete this->depthImage;
}

// public functions
//// getters
std::vector<float> PointCloud::getPointsData()
{
    return *this->pointsData;
}

//// loop function, can either use all images or just selected few passed in array of indexes
void PointCloud::iterateThroughImages(bool imagesAll, int selectedIndexes[], size_t arraySize)
{
    this->pointsData->clear();

    if(!imagesAll)
    {
        for(size_t i = 0; i < arraySize; ++i)
        {
            int index = selectedIndexes[i];
            std::string dir_path = this->inputData->pathToImagesDirectory;

            std::string rgb_image_path = dir_path + this->associationData->rgbData[index];
            std::string depth_image_path = dir_path + this->associationData->depthData[index];

            this->loadRGBImage(rgb_image_path);
            this->loadDepthImage(depth_image_path);

            this->transformToPointCloudData(index);
        }
    }
}

// private functions
//// init functions
void PointCloud::initializeVariables(InputData &input_data)
{
    this->trajectoryData = new std::vector<TrajectoryData>();
    this->associationData = new AssociationData();
    this->inputData = new InputData(input_data);
    this->pointsData = new std::vector<float>();
    this->imageWidth = 0;
    this->imageHeight = 0;

    //camera matrix K
    this->cx = 319.5f;
    this->cy = 239.5f;
    this->focal_x = 481.2f;
    this->focal_y = -480.f;
}

void PointCloud::loadRGBImage(const std::string &path_to_image)
{
    this->rgbImage = new cv::Mat(cv::imread(path_to_image, cv::IMREAD_COLOR));

    if (this->rgbImage->empty())
    {
        std::cerr << "Error loading RBG image!" << path_to_image.c_str() << std::endl;
    }
}

void PointCloud::loadDepthImage(const std::string &path_to_image)
{
    this->depthImage = new cv::Mat(cv::imread(path_to_image, cv::IMREAD_UNCHANGED));

    if (this->depthImage->empty())
    {
        std::cerr << "Error loading Depth image!" << path_to_image.c_str() << std::endl;
        return;
    }

    if (this->depthImage->depth() != CV_16U) {  // or CV_16S if it's signed
        std::cerr << "The image is not 16-bit!" << std::endl;
        return;
    }

    this->imageWidth = this->depthImage->cols;
    this->imageHeight = this->depthImage->rows;
}

void PointCloud::loadTrajectoryData(const std::string &path_to_trajectory)
{
    // Open the file
    std::ifstream file(path_to_trajectory);

    if (!file.is_open())
    {
        std::cerr << "Failed to open trajectory file: " << path_to_trajectory.c_str() << std::endl;
        return;
    }

    std::string line;
    TrajectoryData data;

    // Read and parse the data line by line
    while (std::getline(file, line))
    {
        std::istringstream iss(line);

        // Parse the line into the struct members
        iss >> data.id >> data.cam_x >> data.cam_y >> data.cam_z >> data.qx >> data.qy >> data.qz >> data.qw;

        this->trajectoryData->push_back(data);
    }

    // Close the file
    file.close();
}

void PointCloud::loadAssociationsFile(const std::string &path_to_associations)
{
    // Open the file
    std::ifstream file(path_to_associations);

    if (!file.is_open())
    {
        std::cerr << "Failed to open trajectory file: " << path_to_associations.c_str() << std::endl;
        return;
    }

    std::string line;

    // Read and parse the data line by line
    while (std::getline(file, line))
    {
        std::istringstream iss(line);

        int key;
        std::string value;

        // Parse the line into the struct members
        iss >> key >> value;
        this->associationData->rgbData[key] = value;

        iss >> key >> value;
        this->associationData->depthData[key] = value;
    }

    // Close the file
    file.close();
}

void PointCloud::loadResources()
{
    this->loadTrajectoryData(this->inputData->pathToTrajectoryFile);
    this->loadAssociationsFile(this->inputData->pathToAssociationFile);
}

//// data transformations
void PointCloud::transformToPointCloudData(size_t index)
{
    Eigen::Matrix4f transformation_matrix;
    Eigen::Vector4f position_matrix;
    Eigen::Vector4f transformed_position_matrix;

    for(int v = 0; v < this->imageHeight; ++v)
    {
        for(int u = 0; u < this->imageWidth; ++u)
        {
            //read depth from pixel
            uint16_t read_depth_value = this->depthImage->at<uint16_t>(v, u);

            //read color of pixel
            cv::Vec3b color = this->rgbImage->at<cv::Vec3b>(v, u);

            //casting variables
            float val_d = static_cast<float>(read_depth_value);
            float val_v = static_cast<float>(v);
            float val_u = static_cast<float>(u);

            float blue = static_cast<float>(color[0]);
            float green = static_cast<float>(color[1]);
            float red = static_cast<float>(color[2]);

            //getting real z axis value and scalled x and y values
            float f_d = val_d * 1000 / 65536;
            float f_v = -(val_u - this->cx) / this->focal_x * f_d;
            float f_u = (val_v - this->cy) / this->focal_y * f_d;

            //populating position matrix
            position_matrix << f_u, f_v, f_d, 1;

            //calculating transformation matrix
            float Tx = (*this->trajectoryData)[index].cam_x;
            float Ty = (*this->trajectoryData)[index].cam_y;
            float Tz = (*this->trajectoryData)[index].cam_z;
            float qx = (*this->trajectoryData)[index].qx;
            float qy = (*this->trajectoryData)[index].qy;
            float qz = (*this->trajectoryData)[index].qz;
            float qw = (*this->trajectoryData)[index].qw;

            transformation_matrix << 2 * (qx * qx + qy * qy) - 1, 2 * (qy * qz - qx * qw)    , 2 * (qy * qw + qx * qz)    , Tx,
                                     2 * (qy * qz + qx * qw)    , 2 * (qx * qx + qz * qz) - 1, 2 * (qz * qw - qx * qy)    , Ty,
                                     2 * (qy * qw - qx * qz)    , 2 * (qz * qw + qx * qy)    , 2 * (qx * qx + qw * qw) - 1, Tz,
                                     0                          , 0                          , 0                          , 1;

            //transforming to real point position
            transformed_position_matrix = transformation_matrix * position_matrix;

            //reading point position
            float x = transformed_position_matrix[0];
            float y = transformed_position_matrix[1];
            float z = transformed_position_matrix[2];

            //writing data to points vector
            this->pointsData->insert(pointsData->end(), {x, y, z, red, green, blue});
        }
    }
}
