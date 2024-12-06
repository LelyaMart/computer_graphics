#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>

void multiplyMatrix(double matrix[4][4], double vector[4], double result[4]);

void transformVertices(double vertices[8][3], double transformationMatrix[4][4],
                       double projectedVertices[8][2]);

void set_pixel(cv::Mat& image, int x, int y, const cv::Vec3b& color);

void drawLine(cv::Mat& image, int x1, int y1, int x2, int y2,
              const cv::Vec3b& color);

void calculateWindowSize(double projectedVertices[8][2], int& width,
                         int& height, double& xOffset, double& yOffset);

void transformCoordinates(double x, double y, int& xOut, int& yOut,
                          int windowWidth, int windowHeight, int scale,
                          double minX, double maxY);

void drawPolygon(cv::Mat& image, int vertices[8][2], const cv::Vec3b& color);

void normalize(double& x, double& y, double& z);

double computeNormal(double vertices[4][2]);

void fill(cv::Mat& image, double vertices[][2], int scale,
          const cv::Vec3b& color, double minX, double maxY);
#endif  // UTILS_H
