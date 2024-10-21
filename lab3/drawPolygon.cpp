#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "checkIntersections.h"
#include "drawLine.h"
#include "paintClassification.h"

void drawGrid(cv::Mat& image, int spacing) {
    int width = image.cols;
    int height = image.rows;

    cv::Vec3b gray(200, 200, 200);
    cv::Vec3b black(0, 0, 0);

    int start_x = (width / 2 - (width / 2 / spacing) * spacing);
    int start_y = (height / 2 - (height / 2 / spacing) * spacing);

    for (int x = start_x; x < width; x += spacing) {
        drawLine(image, x, 0, x, height, gray);
    }

    for (int y = start_y; y < height; y += spacing) {
        drawLine(image, 0, y, width, y, gray);
    }

    drawLine(image, width / 2, 0, width / 2, height, black);
    drawLine(image, 0, height / 2, width, height / 2, black);
}

void transformCoordinates(double x, double y, int& xOut, int& yOut,
                          int windowWidth, int windowHeight, int scale) {
    xOut = static_cast<int>(std::round(windowWidth / 2.0 + x * scale));
    yOut = static_cast<int>(std::round(windowHeight / 2.0 - y * scale));
}

void drawPolygon(cv::Mat& image, double vertices[][2], int numVertices,
                 const cv::Vec3b& color, int scale) {
    for (int i = 0; i < numVertices; i++) {
        double x1 = vertices[i][0];
        double y1 = vertices[i][1];

        double x2, y2;
        if (i + 1 < numVertices) {
            x2 = vertices[i + 1][0];
            y2 = vertices[i + 1][1];
        } else {
            x2 = vertices[0][0];
            y2 = vertices[0][1];
        }

        int x1Int, y1Int, x2Int, y2Int;

        transformCoordinates(x1, y1, x1Int, y1Int, image.cols, image.rows,
                             scale);
        transformCoordinates(x2, y2, x2Int, y2Int, image.cols, image.rows,
                             scale);

        drawLine(image, x1Int, y1Int, x2Int, y2Int, color);
    }
}

cv::Size calculateWindowSize(double vertices[][2], int numVertices, int scale) {
    double max_x = 0.0, max_y = 0;

    for (int i = 0; i < numVertices; i++) {
        if (std::abs(vertices[i][0] > max_x)) {
            max_x = std::abs(vertices[i][0]);
        }

        if (std::abs(vertices[i][1] > max_y)) {
            max_y = std::abs(vertices[i][1]);
        }
    }

    int width = static_cast<int>(2 * (max_x + 1) * scale);
    int height = static_cast<int>(2 * (max_y + 1) * scale);

    return cv::Size(width, height);
}

bool hasSelfIntersections(double vertices[][2], int numVertices) {
    double tab, tcd;

    for (int i = 0; i < numVertices; ++i) {
        double ax = vertices[i][0];
        double ay = vertices[i][1];
        double bx = vertices[(i + 1) % numVertices][0];
        double by = vertices[(i + 1) % numVertices][1];

        for (int j = i + 2; j < numVertices; ++j) {
            if (i == 0 && j == numVertices - 1) {
                continue;
            }

            double cx = vertices[j][0];
            double cy = vertices[j][1];
            double dx = vertices[(j + 1) % numVertices][0];
            double dy = vertices[(j + 1) % numVertices][1];

            IntersectType type =
                cross(ax, ay, bx, by, cx, cy, dx, dy, &tab, &tcd);
            if (type == SKEW_CROSS) {
                return true;
            }
        }
    }

    return false;
}

bool isConvex(double vertices[][2], int numVertices) {
    int initialSide = 0;

    for (int i = 0; i < numVertices; ++i) {
        double x1 = vertices[i][0];
        double y1 = vertices[i][1];
        double x2 = vertices[(i + 1) % numVertices][0];
        double y2 = vertices[(i + 1) % numVertices][1];

        double nextX = vertices[(i + 2) % numVertices][0];
        double nextY = vertices[(i + 2) % numVertices][1];

        CLPointType position = classify(x1, y1, x2, y2, nextX, nextY);

        if (position == LEFT || position == RIGHT) {
            int currentSide = (position == LEFT) ? 1 : -1;

            if (initialSide == 0) {
                initialSide = currentSide;
            } else if (initialSide != currentSide) {
                return false;
            }
        }
    }

    return true;
}

void floodFill(cv::Mat& image, double vertices[][2], int numVertices, int scale,
               const cv::Vec3b& color, bool useEO) {
    double min_x = vertices[0][0], min_y = vertices[0][1];
    double max_x = vertices[0][0], max_y = vertices[0][1];

    for (int i = 1; i < numVertices; ++i) {
        min_x = std::min(min_x, vertices[i][0]);
        min_y = std::min(min_y, vertices[i][1]);
        max_x = std::max(max_x, vertices[i][0]);
        max_y = std::max(max_y, vertices[i][1]);
    }

    double step = 1. / scale;

    for (double x = min_x; x <= max_x; x += step) {
        for (double y = min_y; y <= max_y; y += step) {
            PType pointType;
            if (useEO)
                pointType = pInPolygonEOMode(x, y, vertices, numVertices);
            else
                pointType = pInPolygonNZWMode(x, y, vertices, numVertices);
            if (pointType == INSIDE) {
                int xInt, yInt;
                transformCoordinates(x, y, xInt, yInt, image.cols, image.rows,
                                     scale);
                set_pixel(image, xInt, yInt, color);
            }
        }
    }
}

int main() {
    // int numVertices = 4;
    // double vertices[4][2] = {{0, 0}, {0.5, 0}, {0.5, 0.5}, {0, 0.5}};

    // int numVertices = 6;
    // double vertices[6][2] = {{0, 1},    {-2, 0}, {-1, -1},
    //                          {0, -0.5}, {1, -1}, {2, 0}};

    // int numVertices = 6;
    // double vertices[6][2] = {{0, 1}, {-2, 0}, {-1, -1},
    //                          {0, 2}, {1, -1}, {2, 0}};

    int choice;
    std::cout << "Choise mode: \n";
    std::cout << "1 - draw line \n";
    std::cout << "2 - draw emply polygon \n";
    std::cout << "3 - draw polygon with EO filling \n";
    std::cout << "4 - draw polygon with NZW filling \n";
    std::cin >> choice;

    int scale = 100;
    cv::Mat image;

    cv::Vec3b red(0, 0, 255);
    cv::Vec3b green(0, 255, 0);
    cv::Vec3b blue(255, 0, 0);

    if (choice == 1) {
        double x1, y1, x2, y2;
        std::cout << "Enter first point coordinates (x1 y1) \n";
        std::cin >> x1 >> y1;
        std::cout << "Enter second point coordinates (x2 y2) \n";
        std::cin >> x2 >> y2;

        double vertices[][2] = {{x1, y1}, {x2, y2}};
        cv::Size windowSize = calculateWindowSize(vertices, 2, scale);

        image = cv::Mat(windowSize, CV_8UC3, cv::Scalar(255, 255, 255));

        drawGrid(image, scale);

        int x1Int, y1Int, x2Int, y2Int;
        transformCoordinates(x1, y1, x1Int, y1Int, image.cols, image.rows,
                             scale);
        transformCoordinates(x2, y2, x2Int, y2Int, image.cols, image.rows,
                             scale);

        drawLine(image, x1Int, y1Int, x2Int, y2Int, red);
    } else if (choice == 2 || choice == 3 || choice == 4) {
        std::vector<std::pair<double, double>> vertices;
        std::string input;

        std::cout << "Enter polygon vertices(x y). For end type 'end':\n";

        while (true) {
            std::cin >> input;
            if (input == "end") break;
            double x = std::stod(input);
            double y;
            std::cin >> y;
            vertices.push_back({x, y});
        }

        int numVertices = vertices.size();

        double polygonVertices[numVertices][2];
        for (int i = 0; i < numVertices; ++i) {
            polygonVertices[i][0] = vertices[i].first;
            polygonVertices[i][1] = vertices[i].second;
        }

        cv::Size windowSize =
            calculateWindowSize(polygonVertices, numVertices, scale);

        image = cv::Mat(windowSize, CV_8UC3, cv::Scalar(255, 255, 255));

        drawGrid(image, scale);

        cv::Vec3b polygonColor;

        if (hasSelfIntersections(polygonVertices, numVertices)) {
            std::cout << "Polygon has self-intersections \n" << std::endl;
            polygonColor = red;
        } else {
            std::cout << "Polygon has no self-intersections \n" << std::endl;
        }

        if (isConvex(polygonVertices, numVertices)) {
            std::cout << "Polygon is convex \n" << std::endl;
            if (polygonColor != red) polygonColor = green;

        } else {
            std::cout << "Polygon is concave \n" << std::endl;
            if (polygonColor != red) polygonColor = blue;
        }
        drawPolygon(image, polygonVertices, numVertices, polygonColor, scale);

        if (choice == 3 || choice == 4) {
            bool useEO = (choice == 3);
            floodFill(image, polygonVertices, numVertices, scale, polygonColor,
                      useEO);
        }
    }

    cv::imshow("Polygon", image);
    cv::imwrite("./pictures/picture5.png", image);
    cv::waitKey(0);
    return 0;
}
