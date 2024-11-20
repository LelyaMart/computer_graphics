#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

void set_pixel(cv::Mat& image, int x, int y, const cv::Vec3b& color);

void drawLine(cv::Mat& image, int x1, int y1, int x2, int y2,
              const cv::Vec3b& color);

void drawGrid(cv::Mat& image, int spacing);

void transformCoordinates(double x, double y, int& xOut, int& yOut,
                          int windowWidth, int windowHeight, int scale);

void drawPolygon(cv::Mat& image, double vertices[][2], int numVertices,
                 const cv::Vec3b& color, int scale);

int isConvex(double vertices[][2], int numVertices);

#endif  // UTILS_H
