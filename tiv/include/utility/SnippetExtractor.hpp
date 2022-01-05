//
// Created by Redbuzard on 23/11/2020.
//

#ifndef PROJET_OPENCV_CMAKE_SNIPPETEXTRACTOR_HPP
#define PROJET_OPENCV_CMAKE_SNIPPETEXTRACTOR_HPP


#include <opencv2/core/mat.hpp>

/**
 * Class used to extract snippets from images (OpenCV Mat)
 * Snippets are sub-image extracted from the main images
 */
class SnippetExtractor {
public:

    /**
     * Default constructor
     * It only creates the directory ./output/
     */
    SnippetExtractor();


    /**
     * Set the image that will be used for the future extractions
     * @param image a const reference to the OpenCV image
     */
    bool setImage(const cv::Mat& image);


    /**
     * Extract a row of snippets from an image
     * @param the number of the row (starting at 0)
     * @param src a const reference to the source image
     * @param icondID the icon name for the current row
     * @param iconSize the icon size
     * @param scripterNum the scripter number as a string
     * @param pageNum the number of the page loaded for the referred scripter
     */
    void extractRow(uint row, const std::string& iconName, const std::string iconSize,
                    const std::string& scripterNum, const std::string pageNum);


    /**
     * Get the number of rows found
     * @return the rows number as a number
     */
    uint getNumberRows() const;

    /**
     * Get the icon center of a given row
     * @param row the row number
     */
    cv::Point getIconCenter(uint row) const;

    /**
     * Size of a single icon
     * @return the size as a double
     */
    double getIconSize() const;

    /**
     * Extract the first column of a given image
     * @param image of the file
     * @param image to process
     * @return the snippets on the first column
     */
    void getReferences(const cv::Mat &image, std::vector<cv::Mat> &references) const;

    /**
     * Extract the up left ID of a given form
     * @param image of the file
     * @param image to process
     * @return the cropped image with the ID
     */
    void getFormID(const cv::Mat &image, cv::Mat &references) const;

private:
//===============// Private methods //===============//

    /**
     * Get the snippet region for the current snippet
     * @return the snippet region as an OpenCV rotated rect
     */
    cv::RotatedRect snippetRect() const;


    /**
     * Generate a filename to save a snippet
     * @param icondID the icon name for the current row
     * @param scripterNum the scripter number as a string
     * @param pageNum the number of the page loaded for the referred scripter
     * @return the filename as a string
     */
    std::string generateFileName(const std::string& iconName, const std::string& scripterNum, const std::string& pageNum) const;



    /**
     * Save a snippet at a given location
     * @param snippet a const reference to the picture
     * @param path the snippet
     * @param iconName
     * @param iconSize
     * @param scripterNum
     * @param pageNum
     */
    void save(const cv::Mat& snippet, const std::string& path,
              const std::string& iconName, const std::string iconSize,
              const std::string& scripterNum, const std::string pageNum) const;



//===============// Snippet recognition methods //===============//


    /**
     * Find the contours for the current image
     */
    void findSnippetContours();

    /**
     * Find the centers for the current contours
     */
    void findSnippetCenters();

    /**
     * Find the index of the topLeft snippet
     */
    void findTopLeftSnippet();

    /**
     * Find the vectors for constructing the grid
     */
    void findGridVectors();

    /**
     * Build the grid of indexes to find the row and column of each snippet
     */
    void buildIndexGrid();


    /**
     * Get the distance between two points
     * @param p1 a const reference to the first point
     * @param p2 a const reference to the second point
     * @return the squared value of the euclidian distance
     */
    long distance(const cv::Point& p1, const cv::Point& p2) const;


    /**
     * Check if a given snippet bouding box has a square shape
     * @param boundingBox a const reference to the bounding box
     * @return true if the bounding box has a square shape
     */
    bool isSquare(const cv::Rect& boundingBox) const;

    /**
     * Check if a given bounding box has the size of a snippet one
     * @param boundingBox a const reference to the bounding box
     * @return true if it has the correct size
     */
    bool hasSnippetSize(double area) const;


    /**
     * Get the snippet index at a given position on the picture
     * @param point a const reference to the point
     * @return the index of the snippet in the vector or -1 if no one was found
     */
    int getSnippetIndexAt(const cv::Point& point) const;


//===============// Static members //===============//
    // Margin that will be removed from each side of the snippet
    static const unsigned int margin;

    // Error factor allowed when checking size and area
    static const double errorFactor;


//===============// Attributes //===============//
    // Current row and column
    uint m_currentRow, m_currentCol;

    // The Current image
    cv::Mat m_image;

    // The Unchanged image
    cv::Mat m_unchangedImage;

    // The area of a snippet
    double m_snippetArea;

    // Save contours
    std::vector<std::vector<cv::Point> > m_snippetContours;

    // Bounding boxes
    std::vector<cv::Rect> m_boundingBoxes;

    // Contours centers
    std::vector<cv::Point> m_snippetCenters;

    // Matrice of indexes
    std::vector<std::vector<int> > m_indexgrid;


    // Index of the first snippet
    int m_firstSnippet;

    // Vector for finding a snippet on the right or at the bottom
    cv::Point m_vectorRight, m_vectorBottom;
};


#endif //PROJET_OPENCV_CMAKE_SNIPPETEXTRACTOR_HPP
