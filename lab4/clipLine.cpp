#include <iostream>
#include <opencv2/opencv.hpp>

#include "checkIntersections.h"
#include "utils.h"

bool CyrusBeckClipLine(double x1, double y1, double x2, double y2,
                       double vertices[][2], int numVertices, double& x1_new,
                       double& y1_new, double& x2_new, double& y2_new) {
    double t1 = 0, t2 = 1, t;
    double sx = x2 - x1, sy = y2 - y1;
    double nx, ny, denom, num;

    for (int i = 0; i < numVertices; i++) {
        nx = vertices[(i + 1) % numVertices][1] - vertices[i][1];
        ny = vertices[i][0] - vertices[(i + 1) % numVertices][0];

        denom = nx * sx + ny * sy;
        num = nx * (x1 - vertices[i][0]) + ny * (y1 - vertices[i][1]);
        if (denom != 0) {
            t = -num / denom;
            if (denom > 0) {
                if (t > t1) t1 = t;
            } else {
                if (t < t2) t2 = t;
            }
        } else {
            if (classify(vertices[i][0], vertices[i][1],
                         vertices[(i + 1) % numVertices][0],
                         vertices[(i + 1) % numVertices][1], x1, y1) == LEFT)
                return false;
        }
    }

    if (t1 <= t2) {
        x1_new = x1 + t1 * (x2 - x1);
        y1_new = y1 + t1 * (y2 - y1);
        x2_new = x1 + t2 * (x2 - x1);
        y2_new = y1 + t2 * (y2 - y1);
        return true;
    }
    return false;
}

int main() {
    int scale = 100;
    cv::Vec3b blue(255, 0, 0);
    cv::Vec3b red(0, 0, 255);
    cv::Vec3b green(0, 255, 0);

    int numVertices;

    std::cout << "Enter number of polygon vertices: ";
    std::cin >> numVertices;

    double vertices[numVertices][2];
    for (int i = 0; i < numVertices; i++) {
        std::cout << "Enter coordinates for P" << i
                  << " (x y) (available values: "
                     "[0, 4]): ";
        std::cin >> vertices[i][0] >> vertices[i][1];

        if (vertices[i][0] < 0 || vertices[i][0] > 4 || vertices[i][1] < 0 ||
            vertices[i][1] > 4) {
            std::cerr
                << "Invalid coordinates. Please enter values within [0, 4]."
                << std::endl;
            i--;
        }
    }

    int currOrientation = isConvex(vertices, numVertices);

    if (!currOrientation) {
        std::cerr << "Polygon is not convex" << std::endl;
        return -1;
    } else if (currOrientation < 0) {
        for (int i = 0; i < numVertices / 2; ++i) {
            std::swap(vertices[i], vertices[numVertices - 1 - i]);
        }
    }

    double startX, startY, endX, endY;

    std::cout << "Enter coordinates of line's first point (x y) (available "
                 "values: [0, 4]): ";
    std::cin >> startX >> startY;
    if (startX < 0 || startX > 4 || startY < 0 || startY > 4) {
        std::cerr << "Invalid coordinates for line's start" << std::endl;
        return -1;
    }

    std::cout << "Enter coordinates of line's second point (x y) (available "
                 "values: [0, 4]): ";
    std::cin >> endX >> endY;
    if (endX < 0 || endX > 4 || endY < 0 || endY > 4) {
        std::cerr << "Invalid coordinates for line's end" << std::endl;
        return -1;
    }

    cv::Size windowSize = cv::Size(600, 600);

    cv::Mat image = cv::Mat(windowSize, CV_8UC3, cv::Scalar(255, 255, 255));

    drawGrid(image, scale);
    int x1, y1, x2, y2;
    transformCoordinates(startX, startY, x1, y1, image.cols, image.rows, scale);
    transformCoordinates(endX, endY, x2, y2, image.cols, image.rows, scale);

    drawLine(image, x1, y1, x2, y2, green);

    drawPolygon(image, vertices, numVertices, blue, scale);

    double nx1, ny1, nx2, ny2;
    if (CyrusBeckClipLine(startX, startY, endX, endY, vertices, numVertices,
                          nx1, ny1, nx2, ny2)) {
        std::cout << "Clip segment: (" << nx1 << ", " << ny1 << "); (" << nx2
                  << ", " << ny2 << ")" << std::endl;
        int nx1Int, ny1Int, nx2Int, ny2Int;
        transformCoordinates(nx1, ny1, nx1Int, ny1Int, image.cols, image.rows,
                             scale);
        transformCoordinates(nx2, ny2, nx2Int, ny2Int, image.cols, image.rows,
                             scale);
        drawLine(image, nx1Int, ny1Int, nx2Int, ny2Int, red);

    } else {
        std::cout << "No segment inside polygon" << std::endl;
    }

    cv::imshow("Clip line", image);
    // cv::imwrite("./pictures/picture12.png", image);
    cv::waitKey(0);
    return 0;
}