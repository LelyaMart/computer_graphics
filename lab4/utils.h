#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>

void set_pixel(cv::Mat& image, int x, int y, const cv::Vec3b& color);

void drawLine(cv::Mat& image, int x1, int y1, int x2, int y2,
              const cv::Vec3b& color);

cv::Size calculateWindowSize(double vertices[][2], int numVertices, int scale);

void drawGrid(cv::Mat& image, int spacing);

void transformCoordinates(double x, double y, int& xOut, int& yOut,
                          int windowWidth, int windowHeight, int scale);

#endif  // UTILS_H
