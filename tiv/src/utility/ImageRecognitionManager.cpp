//
// Created by Charlotte Nicaudie on 01/12/2020.
//

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"

#include <iostream>
#include <algorithm>
#include <cmath>

#include "utility/ImageRecognitionManager.hpp"
#include "utility/SnippetExtractor.hpp"

#define PI 3.14159265

// Set the constant values
const std::vector<std::string> ImageRecognitionManager::labels =
        {"accident", "bomb", "car", "casualty", "electricity",
         "fire", "fireBrigade", "flood", "gas", "injury",
         "paramedics", "person", "police", "roadBlock"};

const std::vector<std::string> ImageRecognitionManager::sizes = {"large", "medium", "small"};

void ImageRecognitionManager::initImg(const std::string& img) {
    // Loading image into the base of matrix (only done once)
    std::string path = "../base2/" + img + ".png";
    cv::Mat tempMatrix = cv::imread(path);

    // Error management
    if (tempMatrix.data == nullptr) {
        std::cerr << "Image not found - error in ImageRecognitionManager constructor : "<< path << std::endl;
        exit(EXIT_FAILURE);
    }

    if (std::find(labels.begin(), labels.end(), img) != labels.end()) {
        // Adding the matrix to the baseLabels map
        baseLabels.emplace(img, tempMatrix);
    } else if (std::find(sizes.begin(), sizes.end(), img) != sizes.end()) {
        // Adding the matrix to the baseSizes map
        baseSizes.emplace(img, tempMatrix);
    } else {
        std::cerr << "Wrong image name"<< path << std::endl;
        exit(EXIT_FAILURE);
    }

}

ImageRecognitionManager::ImageRecognitionManager() {
    for (const std::string& label : labels) {
        initImg(label);
    }
    for (const std::string& size : sizes) {
        initImg(size);
    }
}

void ImageRecognitionManager::ORBFeaturesDetection(const cv::Mat& referenceImg,
                      const cv::Mat& processImg,
                      std::vector<cv::KeyPoint>& keypoints_object,
                      std::vector<cv::KeyPoint>& keypoints_scene,
                      cv::Mat& descriptors_object,
                      cv::Mat& descriptors_scene) const {
    cv::Ptr<cv::ORB> detector = cv::ORB::create(2000);
    detector->detectAndCompute(referenceImg, cv::noArray(), keypoints_object, descriptors_object);
    detector->detectAndCompute(processImg, cv::noArray(), keypoints_scene, descriptors_scene);
}

void ImageRecognitionManager::loweTestFilter(const std::vector<std::vector<cv::DMatch>>& knn_matches,
                                             std::vector<cv::DMatch>& good_matches) const {
    const float ratio_thresh = 0.75f;
    for (size_t i = 0; i < knn_matches.size(); i++) {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance) {
            good_matches.push_back(knn_matches[i][0]);
        }
    }
}

std::pair<double, double> ImageRecognitionManager::getRatioRotation(const cv::Mat& processImg, const cv::Mat& referenceImg, bool isLabel) const {

    //-- Step 1 : Detect the keypoints using ORB Detector and compute the descriptors
    std::vector<cv::KeyPoint> keypoints_object, keypoints_scene;
    cv::Mat descriptors_object, descriptors_scene;
    ORBFeaturesDetection(referenceImg, processImg, keypoints_object, keypoints_scene, descriptors_object, descriptors_scene);

    //-- Step 2 : Match the descriptor vectors with a Brute-Force Hamming based matcher
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher->knnMatch(descriptors_object, descriptors_scene, knn_matches, 2);

    //-- Step 3 : Filter knn matches using the Lowe's ratio test (keeps only the best matches)
    std::vector<cv::DMatch> good_matches;
    loweTestFilter(knn_matches, good_matches);

    //-- Step 4 : Compute the ratio of the good matches among all matches
    double ratio = ((double) good_matches.size() / (double) knn_matches.size()) * 100;

    //-- Step 5 : Compute the rotation between the reference and the processed images
    double rotation = 90;

    // Need a minimum number of good matches to find the homography matrix
    // No need to compute it on sizes
    if (isLabel && good_matches.size() > 4) {
        // Localize the object
        std::vector<cv::Point2f> obj;
        std::vector<cv::Point2f> scene;
        for (size_t i = 0; i < good_matches.size(); i++) {
            // Get the keypoints from the good matches
            obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
            scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
        }
        // Find the homography matrix
        cv::Mat H = findHomography(obj, scene, cv::RANSAC);

        if (!H.empty()) {
            // Normalize the homography matrix : https://docs.opencv.org/master/d9/dab/tutorial_homography.html
            double norm = sqrt(H.at<double>(0,0)*H.at<double>(0,0) +
                               H.at<double>(1,0)*H.at<double>(1,0) +
                               H.at<double>(2,0)*H.at<double>(2,0));
            H /= norm;

            // Get the angle of rotation between the two images
            // https://stackoverflow.com/questions/15420693/how-to-get-rotation-translation-shear-from-a-3x3-homography-matrix-in-c-sharp
            // https://stackoverflow.com/questions/58538984/how-to-get-the-rotation-angle-from-findhomography
            rotation = atan2(H.at<double>(1,0), H.at<double>(0,0)) * 180 / PI;
        }
    }
    return std::make_pair(ratio, abs(rotation));
}

std::pair<std::string, std::string> ImageRecognitionManager::imageRecognitionAlgorithm(const cv::Mat& processImg) const {
    // Label comparators
    double ratioMaxLabel = 0;
    double rotationMinLabel = 90;
    std::string labelMax;
    // Size comparators
    double ratioMaxSize = 0;
    std::string sizeMax;

    cv::Mat referenceImg;

    for (const std::string& label : labels) {

        //-- Step 1 : Define the reference image (one of the 14 base images) to compare to the image to process
        referenceImg = baseLabels.at(label);

        //-- Step 2 : Compute the ratio of the good matches among all matches
        // + the rotation between the reference & the processed label image
        std::pair<double, double> resRatioRotation = getRatioRotation(processImg, referenceImg, true);
        double tempRatio = resRatioRotation.first;
        double tempRotation = resRatioRotation.second;

        //-- Step 3 : Check if the result is better than the max
        if (tempRatio > ratioMaxLabel) {
            // If so, we check if it has the minimal rotation
            if (tempRotation < rotationMinLabel) {
                // If so, it corresponds to the label that we want
                ratioMaxLabel = tempRatio;
                rotationMinLabel = tempRotation;
                labelMax = label;
            }
        }
    }

    for (const std::string& size : sizes) {

        //-- Step 1 : Define the reference image (one of the 3 base images) to compare to the image to process
        referenceImg = baseSizes.at(size);

        //-- Step 2 : Compute the ratio of the good matches among all matches
        std::pair<double, double> resRatioRotation = getRatioRotation(processImg, referenceImg, false);
        double tempRatio = resRatioRotation.first;

        //-- Step 3 : Check if the result is better than the max and if so, stores the information
        if (tempRatio > ratioMaxSize) {
            ratioMaxSize = tempRatio;
            sizeMax = size;
        }
    }

    // This threshold allows us to determine when the processImg has no size on it
    if (ratioMaxSize < 20) {
        sizeMax = "";
    }

    return std::make_pair(labelMax, sizeMax);
}
