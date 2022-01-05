//
// Created by Charlotte Nicaudie on 16/12/2020.
//

#ifndef PROJET_OPENCV_CMAKE_QUALITYCHECKER_HPP
#define PROJET_OPENCV_CMAKE_QUALITYCHECKER_HPP

#include <string>
#include <map>

#include "utility/DataPathGenerator.hpp"

class QualityChecker {

public:

//===============// Constructor //===============//

    QualityChecker() = default;

//===============// Public methods //===============//

    /**
     * Increments the count of the already seen or not label in parameter
     */
    void putLabel(const std::string& label);

    /**
     * Gets the total count for one label
     * @returns The number of time the label was seen in the base
     */
    int getLabelCount(const std::string& label) const;

    /**
     * Gets the total count for all label
     * @returns The number of time all label were seen in the base
     */
    int getTotalLabels() const;

    /**
     * Gets the total precision for all label
     * @return The precision for all labels
     */
    double getTotalPrecision() const;

    /**
     * Gets the total precision for one label
     * @return The precision for the given label
     */
    double getPrecisionPerLabel(const std::string& label, int initialNumber);

    /**
     * Gets the total recall for all label
     * @return The recall for all labels
     */
    double getTotalRecall() const; // pour tous les labels

    /**
     * Gets the total recall for one label
     * @return The recall for the given label
     */
    double getRecallPerLabel(const std::string& label, int nbBelongingToLabel, int nbCorrectlyAssignedToLabel);

    /**
     * Launches a random check of a result from the output directory
     * Repeats the process for loopLenght
     * */
    void randomCheck(const DataPathGenerator& generator, int loopLength);

private:

//===============// Private attributes //===============//

    // A map associating a label name with the number of time it was seen in the base
    std::map<std::string, int> labelCount;

    // A map associating a label name with its precision result
    std::map<std::string, double> precision;

    // A map associating a label name with its recall result
    std::map<std::string, double> recall;

};


#endif //PROJET_OPENCV_CMAKE_QUALITYCHECKER_HPP
