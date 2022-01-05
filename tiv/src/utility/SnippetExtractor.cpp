//
// Created by Wathis on 23/11/2020.
//

#include "utility/SnippetExtractor.hpp"
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <opencv2/imgproc.hpp>
#include <numeric>
#include <cstdlib>


// Margin
const unsigned int SnippetExtractor::margin = 10;

// Error factor allowed when checking size and area
const double SnippetExtractor::errorFactor = 0.9;



SnippetExtractor::SnippetExtractor() :
m_snippetArea(0){
    // Create the output directory
    mkdir("output", 0777); // 0777 : permission all
}



bool SnippetExtractor::setImage(const cv::Mat &image) {
    // Copy the image
    m_image = image.clone();
    m_unchangedImage= image.clone();

    // Apply Filters
    cv::cvtColor(m_image, m_image, cv::COLOR_BGR2GRAY); // Gray scale
    cv::GaussianBlur(m_image, m_image, cv::Size(3, 3), 0); // Blur
    cv::adaptiveThreshold(m_image, m_image, 255, 1, 1, 11, 15); // Threshold

    // Extract the contours
    findSnippetContours();

    // If there isn't 35 snippets it means that there is a problem with the extraction or the image so we return false
    // (for example : the image n°22 should not have 35 snippets)
    if(m_snippetContours.size() < 10) {
    // if (m_snippetContours.size() != 35) {
        std::cout << "Found " << m_snippetContours.size() << " snippets " << std::endl;
        return false;
    }

    // Find the snippet centers
    findSnippetCenters();

    // Find the index of the topLeft snippet
    findTopLeftSnippet();

    // Find the vectors to construct the grid
    findGridVectors();

    // Build the index grid
    buildIndexGrid();

    return true;
}



void SnippetExtractor::extractRow(uint row, const std::string& iconName, const std::string iconSize,
                                  const std::string &scripterNum, const std::string pageNum) {

    m_currentRow = row;

    if (m_currentRow > m_indexgrid.size()) {
        std::cout << "Only " << m_indexgrid.size() << " rows were found for this image" << std::endl;
        return;
    }

    // For each column in the row
    for(m_currentCol = 0; m_currentCol < m_indexgrid[m_currentRow].size(); m_currentCol++){
        // Get the Region to extract
        cv::RotatedRect rect(snippetRect());

        // matrices we'll use
        cv::Mat M, rotated, cropped;
        // get angle and size from the bounding box
        float angle = rect.angle;
        cv::Size rect_size = rect.size;
        // thanks to http://felix.abecassis.me/2011/10/opencv-rotation-deskewing/
        if (rect.angle < -45.) {
            angle += 90.0;
            cv::swap(rect_size.width, rect_size.height);
        }
        // get the rotation matrix
        M = getRotationMatrix2D(rect.center, angle, 1.0);
        // perform the affine transformation
        warpAffine(m_unchangedImage, rotated, M, m_unchangedImage.size(), cv::INTER_CUBIC);
        // crop the resulting image
        getRectSubPix(rotated, rect_size, rect.center, cropped);

        // Change the extracted rect
        cv::Rect rect2(margin, margin, cropped.cols - 2*margin, cropped.rows - 2*margin);
        cv::Mat res = cropped(rect2);

        // Save the snippet
        std::string savePath(generateFileName(iconName, scripterNum, pageNum));
        save(res, savePath, iconName, iconSize, scripterNum, pageNum);
    }
}



uint SnippetExtractor::getNumberRows() const {
    return m_indexgrid.size();
}



cv::Point SnippetExtractor::getIconCenter(uint row) const {
    // Get the snipept center
    cv::Point snippetCenter(m_snippetCenters[m_indexgrid[row][0]]);
    // Get the icon center
    cv::Point iconCenter;
    iconCenter.x = snippetCenter.x - (m_vectorRight.x + m_vectorBottom.x) * 1.14;
    iconCenter.y = snippetCenter.y - m_vectorRight.y * 1.14;

    return iconCenter;
}



double SnippetExtractor::getIconSize() const {
    return std::abs(m_vectorRight.x) + std::abs(m_vectorBottom.x);
}



cv::RotatedRect SnippetExtractor::snippetRect() const {
    // Get the index of the snippet
    int index = m_indexgrid[m_currentRow][m_currentCol];

    // Get the Rotated rect
    cv::RotatedRect rect(cv::minAreaRect(m_snippetContours[index]));

    // Return the rectangle of minimum area
    return rect;
}



std::string SnippetExtractor::generateFileName(const std::string& iconName, const std::string &scripterNum, const std::string& pageNum) const {
    // Output stream
    std::ostringstream ostr;

    // Input the parameters
    ostr << "output/" << iconName << '_' << scripterNum << '_' << pageNum << '_' << m_currentRow << '_' << m_currentCol;

    // Return the filename
    return ostr.str();
}



void SnippetExtractor::save(const cv::Mat &snippet, const std::string &path, const std::string &iconName,
                            const std::string iconSize, const std::string &scripterNum,
                            const std::string pageNum) const {
    // Save the Snippet picture
    cv::imwrite(path + ".png", snippet);

    // Write the Txt File
    std::ofstream txt(path + ".txt");

    // If the file opened
    if(txt){
        // Putting comments
        txt << "# Groupe 7 | INFO 4" << std::endl;
        txt << "# Members : " << std::endl;
        txt << "# ROBERT   Mathis      |   HU     Romain" << std::endl;
        txt << "# Nicaudie Charlotte   |   MALLAM GABRA Dakini" << std::endl;

        // Putting Data
        txt << "label " << iconName << std::endl;
        txt << "form " << scripterNum << pageNum << std::endl;
        txt << "scripter " << scripterNum << std::endl;
        txt << "page " << pageNum << std::endl;
        txt << "row " << m_currentRow << std::endl;
        txt << "column " << m_currentCol << std::endl;
        txt << "size " << iconSize << std::endl;

        // Close the file
        txt.close();
    }
    else{
        std::cout << "Could not open file " << path << ".txt" << std::endl;
    }
}



void SnippetExtractor::findSnippetContours() {
    // Clear the previous contours
    m_snippetContours.clear();
    m_boundingBoxes.clear();

    // Find the contour with OpenCV
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    findContours(m_image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


    // Find the contour areas
    std::vector<double> areas;
    for(int i = 0 ; i < contours.size(); i++){
        cv::Rect rect(boundingRect(contours[i]));

        areas.push_back(rect.width * rect.height);
    }

    // Sort the areas
    std::vector<double> areasSorted(areas);
    std::sort(areasSorted.begin(), areasSorted.end());
    std::reverse(areasSorted.begin(), areasSorted.end());

    // Get the area of a snippet
    int nbShapes = 5;
    int nbShapesIgnored = 3; // Ignore some of the biggest shapes
    m_snippetArea = std::accumulate(areasSorted.begin()+ nbShapesIgnored, areasSorted.begin() + nbShapes + nbShapesIgnored, 0) / (double)nbShapes;


    // For each contour found
    for(int i = 0 ; i < contours.size(); i++) {
        // Draw only if it is a rectangle
        cv::Rect rect(boundingRect(contours[i]));

        if (isSquare(rect) && hasSnippetSize(rect.width * rect.height)) {
                // Save the contour
                m_snippetContours.push_back(contours[i]);
                m_boundingBoxes.push_back(rect);
        }
    }
}



void SnippetExtractor::findSnippetCenters() {
    // Clear the previous centers
    m_snippetCenters.clear();

    // For each contour, find the bounding box and the center
    for(int i = 0 ; i < m_snippetContours.size(); i++){
        // Find the bouding box
        cv::Rect rect(m_boundingBoxes[i]);

        // Save the center
        cv::Point center(rect.x + rect.width/2., rect.y + rect.height/2.);
        m_snippetCenters.push_back(center);
    }
}



void SnippetExtractor::findTopLeftSnippet() {
    // Anonym struct
    struct SnippetCenter{
        int index;
        int x, y;
    };

    // Create a vector of Snippet centers (with indexes)
    std::vector<SnippetCenter> centers;
    for(int i = 0 ; i < m_snippetCenters.size(); i++) {
        SnippetCenter c;
        c.index = i;
        c.x = m_snippetCenters[i].x;
        c.y = m_snippetCenters[i].y;

        centers.push_back(c);
    }

    // Sort by sum of x and y
    std::sort(centers.begin(), centers.end(),
              [] (const SnippetCenter& p1, const SnippetCenter& p2) {return p1.x+p1.y < p2.x+p2.y;});

    // Take the min as the first snippet
    m_firstSnippet = centers[0].index;
}



// Get the 2 closest centers from the top left snippet
void SnippetExtractor::findGridVectors() {
    // Index of the neighbors and distances
    int neighbor1(0), neighbor2(0);
    long distance1(std::numeric_limits<int>::max()), distance2(std::numeric_limits<int>::max());

    // Position of the first snippet
    cv::Point topLeft = m_snippetCenters[m_firstSnippet];

    // Find the neighbors
    for(int i = 0 ; i < m_snippetCenters.size(); i++){
        // If this snippet center is not the TopLeft one
        if(i != m_firstSnippet){
            long dist = distance(topLeft, m_snippetCenters[i]);

            // Replace neighbor 1
            if(distance1 > dist){
                distance1 = dist;
                neighbor1 = i;
            }
            else if(distance2 > dist){
                distance2 = dist;
                neighbor2 = i;
            }
        }
    }


    // Get the vectors for the grid
    cv::Point p1 = m_snippetCenters[neighbor1];
    cv::Point p2 = m_snippetCenters[neighbor2];

    // P1 is the right snippet
    if(p1.x > p2.x){
        m_vectorRight = p1 - topLeft;
        m_vectorBottom = p2 - topLeft;
    }
    else{
        m_vectorRight = p2 - topLeft;
        m_vectorBottom = p1 - topLeft;
    }
}



void SnippetExtractor::buildIndexGrid() {
    // Booleans for the loops
    bool foundRightSnippet;
    bool foundBottomSnippet;

    // Position of the first snippet center
    cv::Point firstPosition = m_snippetCenters[m_firstSnippet];

    // Add the first snippet
    m_indexgrid.push_back({m_firstSnippet});

    // Row and Column number
    int nRow = 0;
    int nColumn;

    do{
        nColumn = 1;
        do{
            // Try to find a rect
            int nextIndex = getSnippetIndexAt(firstPosition + nRow * m_vectorBottom + nColumn * m_vectorRight);

            if(nextIndex != -1){
                m_indexgrid[nRow].push_back(nextIndex);
                foundRightSnippet = true;
                nColumn++;
            }
            else
                foundRightSnippet = false;
        }while(foundRightSnippet);

        // Go to the next row
        nRow++;

        // Try to find a snippet at the bottom
        int nextIndex = getSnippetIndexAt(firstPosition + nRow * m_vectorBottom);

        if(nextIndex != -1){
            m_indexgrid.push_back({nextIndex});
            foundBottomSnippet = true;
        }
        else
            foundBottomSnippet = false;

    }while(foundBottomSnippet);
}



long SnippetExtractor::distance(const cv::Point &p1, const cv::Point &p2) const {
    long dx = p1.x - p2.x;
    long dy = p1.y - p2.y;

    return dx*dx + dy*dy;
}



bool SnippetExtractor::isSquare(const cv::Rect &boundingBox) const {
    // Height and and width are close
    return (boundingBox.height > errorFactor * boundingBox.width) &&
           (boundingBox.height * errorFactor < boundingBox.width);
}



bool SnippetExtractor::hasSnippetSize(double area) const {
    // The area is close to the supposed snippet size
    return (area > errorFactor * m_snippetArea) && (area * errorFactor < m_snippetArea);
}



int SnippetExtractor::getSnippetIndexAt(const cv::Point &point) const {
    // For each bounding box
    for(int i = 0; i < m_boundingBoxes.size(); i++){
        const cv::Rect& rect = m_boundingBoxes[i];

        // Return the index if it contains the point
        if(rect.contains(point)) {
            return i;
        }
    }

    return -1;
}

void SnippetExtractor::getReferences(const cv::Mat &image, std::vector<cv::Mat> &references) const {
    double width = getIconSize();
    for (int i = 0; i< getNumberRows(); i++) {
        cv::Point center = getIconCenter(i);
        center.x = center.x - (m_vectorRight.x/10);
        cv::Rect crop_region(center.x - width/2, center.y - width/2,width, width);
        references.push_back(image(crop_region));
    }
}

void SnippetExtractor::getFormID(const cv::Mat &image, cv::Mat &references) const {
    double width = getIconSize();
    cv::Point center = getIconCenter(0);
    center.x = center.x - m_vectorRight.x/10;
    center.y = center.y - m_vectorBottom.y;
    cv::Rect crop_region(center.x - width/2, center.y - width/2,width, width);
    references = image(crop_region);
}