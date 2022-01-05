//
// Created by Charlotte Nicaudie on 16/12/2020.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <numeric>

#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"

#include "utility/QualityChecker.hpp"

void QualityChecker::putLabel(const std::string& label) {
    auto it = labelCount.find(label);
    if (it != labelCount.end()) {
       (*it).second += 1;
    } else {
       labelCount[label] = 1;
    }
}

int QualityChecker::getLabelCount(const std::string& label) const {
    int res = 0;
    auto it = labelCount.find(label);
    if (it != labelCount.end()) {
        res = (*it).second;
    }
    return res;
}

int QualityChecker::getTotalLabels() const {
    int sum = 0;
    for (const auto& elem : labelCount) {
        sum += elem.second;
    }
    return sum;
}


void QualityChecker::randomCheck(const DataPathGenerator& generator, int loopLength) {
    int goodMatch = 0;
    std::cout << std::endl;

    // Boucle :
    for (int i = 0; i < loopLength; i++) {

        std::pair<std::string, std::string> paths = generator.randomOutputPath();

        cv::Mat original, snippet;
        original = cv::imread(paths.first);
        snippet = cv::imread("output/" + paths.second + ".png");

        if (original.data == nullptr) {
            std::cerr << "Image not found: " << original << std::endl;
            exit(EXIT_FAILURE);
        }

        if (snippet.data == nullptr) {
            std::cerr << "Image not found: " << snippet << std::endl;
            exit(EXIT_FAILURE);
        }

        // we open the original form
        cv::imshow("QUALITY_CHECK_ORIGINAL", original);

        // we open the snippet to check
        cv::imshow("QUALITY_CHECK_SNIPPET", snippet);

        std::cout << "PRESS ENTER PLEASE !" << std::endl;
        cv::waitKey(0);

        std::cout << std::endl;
        std::cout << "Verify the following text output file considering the original file and the snippet." << std::endl;

        // we open the .txt file corresponding to the snippet
        std::string textFile;
        std::ifstream readFile("output/" + paths.second + ".txt");

        // Use a while loop together with the getline() function to read the file line by line
        while (getline(readFile, textFile)) {
            // Output the text from the file
            std::cout << textFile << std::endl;
        }
        std::cout << std::endl;

        // Close the file
        readFile.close();

        std::cout << "Is everything correct : label/size/id/row/colomn ? (y/n)" << std::endl;

        char rep;
        std::cin >> std::setw(1) >> rep; // setw allows to limit the number of character used

        if (rep == 'y') {
            goodMatch++;
        }
    }
    std::cout << "The algorithm's precision on " << loopLength << " tests images is : " << goodMatch/loopLength << std::endl ;
}


double QualityChecker::getTotalPrecision() const {
    double sumPrecision;
    for( auto it = precision.begin(); it != precision.end(); ++it) {
        sumPrecision += (*it).second;
    }
    return sumPrecision/precision.size();
}

double QualityChecker::getPrecisionPerLabel(const std::string& label, const int numberCorrectlyAssignedToLabel) {
    precision[label] = (double)numberCorrectlyAssignedToLabel/labelCount.at(label);
    return precision.at(label);
}

double QualityChecker::getTotalRecall() const {
    double sumRecall;
    for (auto it = recall.begin(); it != recall.end(); ++it) {
        sumRecall += (*it).second;
    }
    return sumRecall/recall.size();
}

double QualityChecker::getRecallPerLabel(const std::string& label, const int nbBelongingToLabel, const int nbCorrectlyAssignedToLabel) {
    double recallI = (double) nbCorrectlyAssignedToLabel/nbBelongingToLabel;
    recall[label] = recallI;
    return recallI;

}