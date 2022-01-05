//
// Created by Redbuzard on 13/11/2020.
//

#ifndef PROJET_OPENCV_CMAKE_DATAPATHGENERATOR_HPP
#define PROJET_OPENCV_CMAKE_DATAPATHGENERATOR_HPP

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>

/*
 * A Class used to generate all the filename for loading the Data
 */
class DataPathGenerator {
private:
    // Directory from which we'll extract data
    std::string baseDirectory;

    // Directory where the output should be generated
    std::string outputDirectory;

    // A map containing the id of a form and the path to its image
    std::map<std::string, std::string> idToPath;

public:
//===============// Constructor //===============//

    /**
     * Constructor with base path
     */
    DataPathGenerator(const std::string& path, const std::string& outputPath);



//===============// Public methods //===============//

    /**
     * Generates all path to images from the initial base of images
     */
    void generatePath(std::vector<std::string>& output) const;

    /**
     * Adds an element to the map idToPath
     */
    void putPathWithId(const std::string& id, const std::string& path);

    /**
     * Retrieves a random path from a generated random id
     * Used to check a random result
     * @return A pair with the id and the path
     */
    std::pair<std::string, std::string> randomOutputPath() const;

private:

    /**
     * Generates all path to files (either the base directory of the output one)
     */
    void generatePath(std::vector<std::string>& output, bool isBase) const;

    /**
     * Generates a random number between the lowerBound and the upperBound
     */
    int randomNumber(int lowerBound, int upperBound) const;

};


#endif //PROJET_OPENCV_CMAKE_DATAPATHGENERATOR_HPP
