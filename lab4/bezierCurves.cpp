#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "utils.h"

double calculateSegmentCount(double P0[2], double P1[2], double P2[2],
                             double P3[2]) {
    double dist1 =
        fabs(P0[0] - 2 * P1[0] + P2[0]) + fabs(P0[1] - 2 * P1[1] + P2[1]);
    double dist2 =
        fabs(P1[0] - 2 * P2[0] + P3[0]) + fabs(P1[1] - 2 * P2[1] + P3[1]);
    double H = std::max(dist1, dist2);
    return 1. + static_cast<double>(sqrt(3 * H));
}

double factorial(int n) {
    double result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

double bernsteinPolynomial(int i, int m, double t) {
    double binomialCoeff = factorial(m) / (factorial(i) * factorial(m - i));
    return binomialCoeff * pow(t, i) * pow(1 - t, m - i);
}

void bezierPoint(double t, double P[][2], int order, double result[2]) {
    result[0] = result[1] = 0.0;
    for (int i = 0; i <= order; ++i) {
        double B = bernsteinPolynomial(i, order, t);
        result[0] += B * P[i][0];
        result[1] += B * P[i][1];
    }
}

void drawBezierCurve(double P[][2], int order, cv::Mat& image,
                     const cv::Vec3b& color, int scale) {
    double N = calculateSegmentCount(P[0], P[1], P[2], P[3]) * scale;
    double previousPoint[2] = {P[0][0], P[0][1]};
    double currentPoint[2];

    for (int i = 1; i <= N; ++i) {
        double t = static_cast<double>(i) / N;
        bezierPoint(t, P, order, currentPoint);
        int x1Int, y1Int, x2Int, y2Int;
        transformCoordinates(previousPoint[0], previousPoint[1], x1Int, y1Int,
                             image.cols, image.rows, scale);
        transformCoordinates(currentPoint[0], currentPoint[1], x2Int, y2Int,
                             image.cols, image.rows, scale);
        drawLine(image, x1Int, y1Int, x2Int, y2Int, color);
        previousPoint[0] = currentPoint[0];
        previousPoint[1] = currentPoint[1];
    }
}

int main() {
    int order = 3;
    int scale = 100;

    cv::Vec3b blue(255, 0, 0);

    double P[order + 1][2];
    for (int i = 0; i <= order; ++i) {
        std::cout << "Enter coordinates for P" << i << " (x y): ";
        std::cin >> P[i][0] >> P[i][1];
    }

    cv::Size windowSize = cv::Size(600, 600);

    cv::Mat image = cv::Mat(windowSize, CV_8UC3, cv::Scalar(255, 255, 255));

    drawGrid(image, scale);

    drawBezierCurve(P, order, image, blue, scale);
    cv::imshow("Bezier curve", image);
    // cv::imwrite("./pictures/picture5.png", image);
    cv::waitKey(0);
    return 0;
}