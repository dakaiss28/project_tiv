//
// Created by Charlotte Nicaudie on 01/12/2020.
//

#include "utility/TextExtractionManager.hpp"

TextExtractionManager::TextExtractionManager() {}

std::string TextExtractionManager::TextExtractionAlgorithm(cv::Mat& referenceImg) {
    std::string output;

    // We create an OCR object
    cv::Ptr<cv::text::OCRTesseract> ocr = cv::text::OCRTesseract::create(nullptr, "eng","0123456789",
                                                                         cv::text::OEM_DEFAULT,cv::text::PSM_AUTO);

    // And we run the OCR on the reference image and get the result string in output
    ocr->run(referenceImg, output,nullptr, nullptr,
             nullptr, cv::text::OCR_LEVEL_WORD);

    return output;
}
