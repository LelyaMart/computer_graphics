#include <array>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "checkIntersections.h"
#include "utils.h"

bool areVerticesOnEdges(double verticesFirst[][2], double verticesSecond[][2],
                        int numVerticesFirst, int numVerticesSecond) {
    for (int j = 0; j < numVerticesSecond; j++) {
        for (int i = 0; i < numVerticesFirst; i++) {
            if (isPointOnSegment(verticesFirst[i][0], verticesFirst[i][1],
                                 verticesFirst[(i + 1) % numVerticesFirst][0],
                                 verticesFirst[(i + 1) % numVerticesFirst][1],
                                 verticesSecond[j][0], verticesSecond[j][1])) {
                return true;
            }
        }
    }
    return false;
}

struct Point {
    double x, y;
    bool isInside;
    bool isIntersection;
    double t;
    bool isEntering;

    Point(double x, double y, bool isInside = false,
          bool isIntersection = false, double t = 0, bool isEntering = false)
        : x(x),
          y(y),
          isInside(isInside),
          isIntersection(isIntersection),
          t(t),
          isEntering(isEntering) {}
};

double segmentIntersection(double p1x, double p1y, double p2x, double p2y,
                           double q1x, double q1y, double q2x, double q2y) {
    double dx1 = p2x - p1x;
    double dy1 = p2y - p1y;
    double dx2 = q2x - q1x;
    double dy2 = q2y - q1y;

    double det = dx1 * dy2 - dy1 * dx2;
    if (det == 0) {
        return -1;
    }

    double detT = (q1x - p1x) * dy2 - (q1y - p1y) * dx2;
    double detU = (q1x - p1x) * dy1 - (q1y - p1y) * dx1;

    double t = detT / det;
    double u = detU / det;

    if (t > 0 && t < 1 && u > 0 && u < 1) {
        return t;
    }

    return -1;
}

bool isPointInPolygon(double x, double y, double vertices[][2],
                      int numVertices) {
    int intersections = 0;

    for (int i = 0; i < numVertices; ++i) {
        double x1 = vertices[i][0];
        double y1 = vertices[i][1];
        double x2 = vertices[(i + 1) % numVertices][0];
        double y2 = vertices[(i + 1) % numVertices][1];

        if (((y1 <= y && y < y2) || (y2 <= y && y < y1)) &&
            (x < ((x2 - x1) * (y - y1)) / (y2 - y1) + x1)) {
            intersections++;
        }
    }

    return intersections % 2 == 1;
}

std::vector<Point> buildCyclicList(double vertices[][2], int numVertices,
                                   double secVertices[][2], int secNumVertices,
                                   int& numIntersections) {
    std::vector<Point> cyclicList;

    for (int i = 0; i < numVertices; i++) {
        bool isInside = isPointInPolygon(vertices[i][0], vertices[i][1],
                                         secVertices, secNumVertices);
        Point p1(vertices[i][0], vertices[i][1], isInside);
        Point p2(vertices[(i + 1) % numVertices][0],
                 vertices[(i + 1) % numVertices][1]);
        cyclicList.push_back(p1);

        std::vector<Point> intersections;

        for (int j = 0; j < secNumVertices; j++) {
            double t = segmentIntersection(
                p1.x, p1.y, p2.x, p2.y, secVertices[j][0], secVertices[j][1],
                secVertices[(j + 1) % secNumVertices][0],
                secVertices[(j + 1) % secNumVertices][1]);
            if (t > 0) {
                double x = p1.x + t * (p2.x - p1.x);
                double y = p1.y + t * (p2.y - p1.y);
                bool isInside =
                    isPointInPolygon(x, y, secVertices, secNumVertices);
                Point intersection(x, y, isInside, true, t, false);
                intersections.push_back(intersection);
                numIntersections++;
            }
        }
        std::sort(intersections.begin(), intersections.end(),
                  [](const Point& a, const Point& b) { return a.t < b.t; });

        for (const auto& intersection : intersections) {
            cyclicList.push_back(intersection);
        }
    }

    return cyclicList;
}

std::vector<Point> weilerAzertonAlgorithm(double verticesFirst[][2],
                                          double verticesSecond[][2],
                                          int numVerticesFirst,
                                          int numVerticesSecond) {
    int numIntersections1 = 0, numIntersections2 = 0;

    std::vector<Point> resultPolygons;

    auto cyclicList1 =
        buildCyclicList(verticesFirst, numVerticesFirst, verticesSecond,
                        numVerticesSecond, numIntersections1);
    auto cyclicList2 =
        buildCyclicList(verticesSecond, numVerticesSecond, verticesFirst,
                        numVerticesFirst, numIntersections2);

    if (numIntersections1 == 0) {
        if (isPointInPolygon(verticesFirst[0][0], verticesFirst[0][1],
                             verticesSecond, numVerticesSecond)) {
            return {cyclicList1};
        } else {
            return {};
        }
    }

    std::array<std::vector<Point>, 2> lists = {cyclicList1, cyclicList2};
    int currList = 0;
    int checkedVertices = 0;
    int currIdx = 0;

    do {
        Point& currentPoint = lists[currList][currIdx];

        if (!currentPoint.isEntering) {
            if (currentPoint.isIntersection && !resultPolygons.empty()) {
                resultPolygons.push_back(currentPoint);
                currentPoint.isEntering = true;
                currList = (currList + 1) % 2;
                for (int i = 0; i < lists[currList].size(); i++) {
                    if (lists[currList][i].x == currentPoint.x &&
                        lists[currList][i].y == currentPoint.y) {
                        currIdx = i;
                        lists[currList][i].isEntering = true;
                        checkedVertices++;
                        currIdx = (currIdx + 1) % lists[currList].size();
                        break;
                    }
                }
            } else {
                if (currentPoint.isInside) {
                    resultPolygons.push_back(currentPoint);
                    currentPoint.isEntering = true;
                }
                currIdx = (currIdx + 1) % lists[currList].size();
            }
        }
    } while (resultPolygons.empty() ||
             lists[currList][currIdx].x != resultPolygons[0].x ||
             lists[currList][currIdx].y != resultPolygons[0].y);
    return resultPolygons;
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
        std::cout << "Enter coordinates for vertices"
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

    if (hasSelfIntersections(vertices, numVertices)) {
        std::cerr << "Polygon has self-intersections";
        return -1;
    }

    int numCutVertices;

    std::cout << "Enter number of polygon cut-off vertices: ";
    std::cin >> numCutVertices;

    double verticesCut[numCutVertices][2];
    for (int i = 0; i < numCutVertices; i++) {
        std::cout << "Enter coordinates for vertices"
                  << " (x y) (available values: "
                     "[0, 4]): ";
        std::cin >> verticesCut[i][0] >> verticesCut[i][1];

        if (verticesCut[i][0] < 0 || verticesCut[i][0] > 4 ||
            verticesCut[i][1] < 0 || verticesCut[i][1] > 4) {
            std::cerr
                << "Invalid coordinates. Please enter values within [0, 4]."
                << std::endl;
            i--;
        }
    }

    if (hasSelfIntersections(verticesCut, numCutVertices)) {
        std::cerr << "Polygon has self-intersections";
        return -1;
    }

    if (!checkOrientation(vertices)) {
        for (int i = 0; i < numVertices / 2; ++i) {
            std::swap(vertices[i], vertices[numVertices - 1 - i]);
        }
    }

    if (!checkOrientation(verticesCut)) {
        for (int i = 0; i < numCutVertices / 2; ++i) {
            std::swap(verticesCut[i], verticesCut[numCutVertices - 1 - i]);
        }
    }

    if (areVerticesOnEdges(vertices, verticesCut, numVertices,
                           numCutVertices) ||
        areVerticesOnEdges(verticesCut, vertices, numCutVertices,
                           numVertices)) {
        std::cerr << "One polygon has vertices on other's edges" << std::endl;
        return -1;
    }

    std::vector<Point> clipPolygons = weilerAzertonAlgorithm(
        vertices, verticesCut, numVertices, numCutVertices);

    cv::Size windowSize = cv::Size(600, 600);

    cv::Mat image = cv::Mat(windowSize, CV_8UC3, cv::Scalar(255, 255, 255));

    drawGrid(image, scale);
    drawPolygon(image, vertices, numVertices, blue, scale);
    drawPolygon(image, verticesCut, numCutVertices, green, scale);

    if (clipPolygons.empty()) {
        std::cout << "Intersection is empty set";
    } else {
        double polygon_vertices[clipPolygons.size()][2];
        for (int i = 0; i < clipPolygons.size(); i++) {
            polygon_vertices[i][0] = clipPolygons[i].x;
            polygon_vertices[i][1] = clipPolygons[i].y;
        }
        drawPolygon(image, polygon_vertices, clipPolygons.size(), red, scale);
    }

    cv::imshow("Clip polygon", image);
    cv::imwrite("./pictures/picture5.png", image);
    cv::waitKey(0);
    return 0;
}