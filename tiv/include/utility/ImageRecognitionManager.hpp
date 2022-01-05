//
// Created by Charlotte Nicaudie on 01/12/2020.
//

#ifndef PROJET_OPENCV_CMAKE_IMAGERECOGNITIONMANAGER_HPP
#define PROJET_OPENCV_CMAKE_IMAGERECOGNITIONMANAGER_HPP

#include <string>
#include <map>

/*
 * A Class used to determine the label and the size of an image
 */
class ImageRecognitionManager {
public:
//===============// Constructor //===============//

    /**
     * Default constructor
     * Builds the base of the 42 matrix (loads each image from the base)
     */
    ImageRecognitionManager();

//===============// Public methods //===============//

    /**
     * Search for the best corresponding image from the base in comparison of the image to process
     * @return a pair of strings containing :
     * first : the label of the recognize image
     * second : its size
     */
    std::pair<std::string, std::string>  imageRecognitionAlgorithm(const cv::Mat& processImg) const;

    /**
     * Getter for the labels
     */
    inline std::vector<std::string> getLabels() const {
        return labels;
    }

private:

//===============// Private constants //===============//

    // Names of all known labels
    static const std::vector<std::string> labels;

    // Names of all possible sizes
    static const std::vector<std::string> sizes;

//===============// Attributes //===============//

    // Map associating the name of a label with its matrix
    std::map<std::string, cv::Mat> baseLabels;

    // Map associating the name of a size with its matrix
    std::map<std::string, cv::Mat> baseSizes;

//===============// Private methods //===============//

    /**
     * Initializes the databases of labels and and sizes
     */
    void initImg(const std::string& img);

    /**
     * Gets the ratio of good match among all matches of keypoints detection and the rotation between the process and reference image
     * @return A pair containing the ratio and the rotation
     */
    std::pair<double, double> getRatioRotation(const cv::Mat& processImg, const cv::Mat& referenceImg, bool isLabel) const;

    /**
     * Detects and computes the features and descriptors of the reference image and the one to process - using ORB algorithm
     */
    void ORBFeaturesDetection(const cv::Mat& referenceImg,
                              const cv::Mat& processImg,
                              std::vector<cv::KeyPoint>& keypoints_object,
                              std::vector<cv::KeyPoint>& keypoints_scene,
                              cv::Mat& descriptors_object,
                              cv::Mat& descriptors_scene) const;

    /**
     * Uses the Lowe's test filter to select the best matches
     */
    void loweTestFilter(const std::vector<std::vector<cv::DMatch>>& knn_matches, std::vector<cv::DMatch>& good_matches) const;

};


#endif //PROJET_OPENCV_CMAKE_IMAGERECOGNITIONMANAGER_HPP
