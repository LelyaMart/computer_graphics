#ifndef DRAW_LINE_H
#define DRAW_LINE_H

#include <opencv2/opencv.hpp>

void set_pixel(cv::Mat& image, int x, int y, const cv::Vec3b& color);

void drawLine(cv::Mat& image, int x1, int y1, int x2, int y2, const cv::Vec3b& color);

#endif // DRAW_LINE_H
