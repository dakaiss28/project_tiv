//
// Created by Redbuzard on 13/11/2020.
//

#include "utility/DataPathGenerator.hpp"
#include <regex>
#include <random>

namespace fs = std::__fs::filesystem;

DataPathGenerator::DataPathGenerator(const std::string& path, const std::string& outputPath) : baseDirectory(path), outputDirectory(outputPath), idToPath() {
    std::cout << "Deleted " << fs::remove_all(outputPath) << " files or directories" << std::endl;
}


void DataPathGenerator::generatePath(std::vector<std::string>& output, bool isBase) const {
    auto& dir = isBase ? baseDirectory : outputDirectory;

    for (const auto & entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_directory() || std::string(entry.path()).find(".DS_Store") != std::string::npos) {
            continue; // we skip if it is a directory or a special file (mac)
        }
        output.emplace_back(entry.path());
    }
}

void DataPathGenerator::generatePath(std::vector<std::string>& output) const {
    generatePath(output, true);
}

void DataPathGenerator::putPathWithId(const std::string& id, const std::string& path) {
    idToPath.emplace(id, path);
}

int DataPathGenerator::randomNumber(int lowerBound, int upperBound) const {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> randomValue(lowerBound,upperBound);
    return randomValue(rng);
}


std::pair<std::string, std::string> DataPathGenerator::randomOutputPath() const {
    // we choose a random id among all forms
    int rand = randomNumber(0, idToPath.size() - 1);
    auto it = idToPath.begin();
    std::advance(it, rand);
    std::string id = (*it).first;
    std::string path = (*it).second;

    std::cout << "ID : " << id << std::endl;
    std::cout << "PATH : " << path << std::endl;

    // we get all path in the output directory
    std::vector<std::string> output;
    generatePath(output, false);

    // we use regular expression to choose one of the snippet among all outputs
    std::smatch m;
    std::string regex = "([a-zA-Z]+_"+ id.substr(0, 3) +"_" + id.substr(3, 2) + "_[0-9]_[0-9])";
    std::regex e(regex);
    std::vector<std::string> goodMatches;

    // we go through all path
    for (const std::string& s : output) {
        // if we get a match
        if (std::regex_search (s,m,e)) {
            // we get the first one (it is producing 2 results bc of the .png and .txt)
            goodMatches.push_back(m[0]); // and push it into the good matches vector
        }
    }

    // then we get another random number to get 1 random snippet
    rand = randomNumber(0, goodMatches.size() - 1);

    // and finally, we return the corresponding base path
    std::cout << goodMatches[rand] << std::endl;

    return std::make_pair(path, goodMatches[rand]);
}

