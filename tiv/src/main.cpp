#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include "opencv2/imgcodecs.hpp"
using namespace cv;

#include "utility/ImageRecognitionManager.hpp"
#include "utility/TextExtractionManager.hpp" // wasn't required at the end of the project
#include <utility/SnippetExtractor.hpp>
#include "utility/DataPathGenerator.hpp"
#include "utility/QualityChecker.hpp"

void openImage(const std::string& path, cv::Mat& res) {
    res = imread(path);
    if (res.data == nullptr){
        std::cerr << "Image not found: "<< path << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main () {

    /** STEP 0 : We setup the quality checker and start the chrono - for performances measures **/
    QualityChecker checker;
    auto start = std::chrono::steady_clock::now();

    /** STEP 1 : Get all image path from the base **/
    DataPathGenerator generator("donnees", "output");

    std::vector<std::string> pathToImages;
    generator.generatePath(pathToImages);

    std::cout << "Number of images to process : " << pathToImages.size() << std::endl;

    /** STEP 3 : For a given image :
     * Extract the ID of the form
     * Recognize it using the text extraction manager
     * Extract the reference labels (and size if present)
     * For each row :
     *  - Recognize the reference label + size using the image recognition manager
     *  - Extract the snippets on the row with the given label + size
     *  - Repeat
     * Repeat the process until all image are processed
    **/

    //TextExtractionManager textManager;
    ImageRecognitionManager imgManager;
    std::string formIdText;
    // cv::Mat formId;

    for (std::string& img : pathToImages) {

        // Open the current image and put it in a matrix
        cv::Mat m;
        openImage(img, m);

        // Set the image on which we extract the informations
        SnippetExtractor extractor;

        // Skip images with no snippets
        if (!extractor.setImage(m)) {
            std::cout << "Skipped : " << img << std::endl;
            continue;
        }

        // Extract the ID of the form
        //extractor.getFormID(m, formId);

        // Recognize it using the text extraction manager
        //formIdText = textManager.TextExtractionAlgorithm(formId).substr(0, 5);

        std::string formIdText;
        for (auto i=0 ; i < img.length(); i++ ){ if ( isdigit(img[i]) ) formIdText+=img[i]; }


        // Add it to the idToPath map
        generator.putPathWithId(formIdText, img);

        // Extract the reference label (and size if present)
        std::vector<cv::Mat> references;
        extractor.getReferences(m, references);

        // For each row
        for (int j = 0; j < extractor.getNumberRows(); j++) {
            // Recognize the reference label + size using the image recognition manager
            std::pair<std::string, std::string> rowLabelSize = imgManager.imageRecognitionAlgorithm(references[j]);

            // Counting labels in the quality checker
            if(!rowLabelSize.first.empty())
            checker.putLabel(rowLabelSize.first);

            // Extract the snippets on the row with the given label + size
            if(!rowLabelSize.first.empty())
            extractor.extractRow(j, rowLabelSize.first, rowLabelSize.second, formIdText.substr(0, 2), formIdText.substr(2, 4));
        }
    }

    /** STEP 4 : We check the algorithm performances **/
    std::cout << "==========================" << std::endl;
    std::cout << "==== QUALITY  SECTION ====" << std::endl;
    std::cout << "==========================" << std::endl;

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Execution duration : " << elapsed_seconds.count() << " sec" << std::endl;
    std::cout << "==========================" << std::endl;

}
