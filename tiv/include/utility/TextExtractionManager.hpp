//
// Created by Charlotte Nicaudie on 01/12/2020.
//

#ifndef PROJET_OPENCV_CMAKE_TEXTEXTRACTIONMANAGER_HPP
#define PROJET_OPENCV_CMAKE_TEXTEXTRACTIONMANAGER_HPP

#include <string>

#include <opencv2/text/ocr.hpp>


/*
 * A Class used to handle the text extraction (to get the ID of a given form)
 */
class TextExtractionManager {

public:
//===============// Constructor //===============//

    /**
     * Default constructor
     */
    TextExtractionManager();

//===============// Public methods //===============//

    /**
     * Apply the OCR algorithm on the reference image
     * @return the extracted text
     */
    std::string TextExtractionAlgorithm(cv::Mat& referenceImg);

};


#endif //PROJET_OPENCV_CMAKE_TEXTEXTRACTIONMANAGER_HPP
