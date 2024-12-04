#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "utils.h"

void drawVector(double startX, double startY, double u, double v, cv::Mat image,
                cv::Vec3b color, int scale) {
    double vecLen = sqrt(u * u + v * v);
    u = u / vecLen / 2;
    v = v / vecLen / 2;
    int x1, x2, y1, y2;
    transformCoordinates(startX, startY, x1, y1, image.cols, image.rows, scale);
    transformCoordinates(startX + u, startY + v, x2, y2, image.cols, image.rows,
                         scale);
    drawLine(image, x1, y1, x2, y2, color);
}

void hermitePoint(double t, double x1, double y1, double x2, double y2,
                  double u1, double v1, double u2, double v2,
                  double result[2]) {
    result[0] = (1 - 3 * pow(t, 2) + 2 * pow(t, 3)) * x1 +
                pow(t, 2) * (3 - 2 * t) * x2 +
                t * (1 - 2 * t + pow(t, 2)) * u1 - pow(t, 2) * (1 - t) * u2;
    result[1] = (1 - 3 * pow(t, 2) + 2 * pow(t, 3)) * y1 +
                pow(t, 2) * (3 - 2 * t) * y2 +
                t * (1 - 2 * t + pow(t, 2)) * v1 - pow(t, 2) * (1 - t) * v2;
}

void drawSingleHermiteCurve(double x1, double y1, double x2, double y2,
                            double u1, double v1, double u2, double v2,
                            cv::Mat& image, const cv::Vec3b& color, int scale) {
    double N = 100.0;
    double previousPoint[2] = {x1, y1};
    double currentPoint[2];

    for (int i = 1; i <= N; i++) {
        double t = static_cast<double>(i) / N;
        hermitePoint(t, x1, y1, x2, y2, u1, v1, u2, v2, currentPoint);
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

void drawHermiteCurves(double vertices[][2], double vectors[][2], int num,
                       cv::Mat image, cv::Vec3b color, int scale) {
    for (int i = 0; i < num; i++) {
        drawSingleHermiteCurve(vertices[i][0], vertices[i][1],
                               vertices[i + 1][0], vertices[i + 1][1],
                               vectors[i][0], vectors[i][1], vectors[i + 1][0],
                               vectors[i + 1][1], image, color, scale);
    }
}

int main() {
    int scale = 100;
    cv::Vec3b blue(255, 0, 0);
    cv::Vec3b green(0, 255, 0);

    int num;
    std::cout
        << "Enter number of curve's parts (number of points will be n + 1): ";
    std::cin >> num;

    if (num < 1) {
        std::cerr << "Number of parts should be natural number";
        return -1;
    }

    double vertices[num + 1][2];
    double vectors[num + 1][2];

    for (int i = 0; i <= num; i++) {
        std::cout << "Enter points coordinates (x y): ";
        std::cin >> vertices[i][0] >> vertices[i][1];
    }

    for (int i = 0; i <= num; i++) {
        std::cout << "Enter vector coordinates (x y): ";
        std::cin >> vectors[i][0] >> vectors[i][1];
    }

    cv::Size imageSize = calculateWindowSize(vertices, num + 1, scale);

    cv::Mat image = cv::Mat(imageSize, CV_8UC3, cv::Scalar(255, 255, 255));

    drawGrid(image, scale);
    for (int i = 0; i < num + 1; i++) {
        drawVector(vertices[i][0], vertices[i][1], vectors[i][0], vectors[i][1],
                   image, green, scale);
    }

    drawHermiteCurves(vertices, vectors, num, image, blue, scale);

    cv::imshow("Hermite curves", image);
    // cv::imwrite("./pictures/picture4.png", image);
    cv::waitKey(0);
}
