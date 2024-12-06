#include "utils.h"

void multiplyMatrix(double matrix[4][4], double vector[4], double result[4]) {
    for (int i = 0; i < 4; i++) {
        result[i] = 0;
        for (int j = 0; j < 4; j++) {
            result[i] += vector[j] * matrix[j][i];
        }
    }
}

void transformVertices(double vertices[8][3], double transformationMatrix[4][4],
                       double projectedVertices[8][2]) {
    for (int i = 0; i < 8; i++) {
        double homogeneousVertex[4] = {vertices[i][0], vertices[i][1],
                                       vertices[i][2], 1};
        double transformedVertex[4];

        multiplyMatrix(transformationMatrix, homogeneousVertex,
                       transformedVertex);

        double h = transformedVertex[3];

        projectedVertices[i][0] = transformedVertex[0] / h;
        projectedVertices[i][1] = transformedVertex[1] / h;
    }
}

void set_pixel(cv::Mat& image, int x, int y, const cv::Vec3b& color) {
    if (x >= 0 && x < image.cols && y >= 0 && y < image.rows) {
        image.at<cv::Vec3b>(y, x) = color;
    }
}

void drawLine(cv::Mat& image, int x1, int y1, int x2, int y2,
              const cv::Vec3b& color) {
    int x = x1;
    int y = y1;

    int dx = x2 - x1;
    int dy = y2 - y1;

    int ix, iy;

    int e;
    int i;

    if (dx > 0)
        ix = 1;
    else if (dx < 0) {
        ix = -1;
        dx = -dx;
    } else
        ix = 0;

    if (dy > 0)
        iy = 1;
    else if (dy < 0) {
        iy = -1;
        dy = -dy;
    } else
        iy = 0;

    if (dx >= dy) {
        e = 2 * dy - dx;
        if (iy > 0) {
            for (i = 0; i < dx; i++) {
                set_pixel(image, x, y, color);
                if (e >= 0) {
                    y += iy;
                    e -= 2 * dx;
                }
                x += ix;
                e += dy * 2;
            }
        } else {
            for (i = 0; i <= dx; i++) {
                set_pixel(image, x, y, color);
                if (e > 0) {
                    y += iy;
                    e -= 2 * dx;
                }
                x += ix;
                e += 2 * dy;
            }
        }
    } else {
        e = 2 * dx - dy;
        if (ix > 0) {
            for (i = 0; i < dy; i++) {
                set_pixel(image, x, y, color);
                if (e >= 0) {
                    x += ix;
                    e -= 2 * dy;
                }
                y += iy;
                e += dx * 2;
            }
        } else {
            for (i = 0; i <= dy; i++) {
                set_pixel(image, x, y, color);
                if (e > 0) {
                    x += ix;
                    e -= 2 * dy;
                }
                y += iy;
                e += 2 * dx;
            }
        }
    }
}

void calculateWindowSize(double projectedVertices[8][2], int& width,
                         int& height, double& xOffset, double& yOffset) {
    double xMin = projectedVertices[0][0], xMax = projectedVertices[0][0];
    double yMin = projectedVertices[0][1], yMax = projectedVertices[0][1];

    for (int i = 1; i < 8; ++i) {
        xMin = std::min(xMin, projectedVertices[i][0]);
        xMax = std::max(xMax, projectedVertices[i][0]);
        yMin = std::min(yMin, projectedVertices[i][1]);
        yMax = std::max(yMax, projectedVertices[i][1]);
    }

    xMin -= 1.0f;
    xMax += 1.0f;
    yMin -= 1.0f;
    yMax += 1.0f;

    width = static_cast<int>(std::ceil(xMax - xMin));
    height = static_cast<int>(std::ceil(yMax - yMin));

    xOffset = xMin;
    yOffset = yMax;
}

void transformCoordinates(double x, double y, int& xOut, int& yOut,
                          int windowWidth, int windowHeight, int scale,
                          double minX, double maxY) {
    xOut = static_cast<int>((x - minX) * scale);
    yOut = static_cast<int>((maxY - y) * scale);
}

void drawPolygon(cv::Mat& image, int vertices[8][2], const cv::Vec3b& color) {
    for (int i = 0; i < 4; i++) {
        int x1 = vertices[i][0];
        int y1 = vertices[i][1];

        int x3 = vertices[i + 4][0];
        int y3 = vertices[i + 4][1];

        int x2, y2, x4, y4;
        if (i + 1 < 4) {
            x2 = vertices[i + 1][0];
            y2 = vertices[i + 1][1];

            x4 = vertices[i + 5][0];
            y4 = vertices[i + 5][1];

        } else {
            x2 = vertices[0][0];
            y2 = vertices[0][1];

            x4 = vertices[4][0];
            y4 = vertices[4][1];
        }

        drawLine(image, x1, y1, x2, y2, color);
        drawLine(image, x3, y3, x4, y4, color);
        drawLine(image, x1, y1, x3, y3, color);
    }
}

void normalize(double& x, double& y, double& z) {
    double l = std::sqrt(x * x + y * y + z * z);
    x /= l;
    y /= l;
    z /= z;
}

double computeNormal(double vertices[4][2]) {
    double x1 = vertices[0][0], y1 = vertices[0][1];
    double x2 = vertices[1][0], y2 = vertices[1][1];
    double x3 = vertices[2][0], y3 = vertices[2][1];

    double a1 = x2 - x1, a2 = y2 - y1;
    double b1 = x3 - x1, b2 = y3 - y1;

    double nz = a1 * b2 - a2 * b1;

    return nz;
}

enum CLPointType { LEFT, RIGHT, BEYOND, BEHIND, BETWEEN, ORIGIN, DESTINATION };

enum EType { TOUCHING, CROSS_LEFT, CROSS_RIGHT, INESSENTIAL };

enum PType { INSIDE, OUTSIDE };

CLPointType classify(double x1, double y1, double x2, double y2, double x,
                     double y) {
    double ax = x2 - x1;
    double ay = y2 - y1;

    double bx = x - x1;
    double by = y - y1;

    double s = ax * by - bx * ay;

    if (s > 0) return LEFT;
    if (s < 0) return RIGHT;

    if ((ax * bx < 0) || (ay * by < 0)) return BEHIND;
    if ((ax * ax + ay * ay) < (bx * bx + by * by)) return BEYOND;

    if (x1 == x && y1 == y) return ORIGIN;
    if (x2 == x && y2 == y) return DESTINATION;

    return BETWEEN;
}

EType edgeType(double ox, double oy, double dx, double dy, double ax,
               double ay) {
    switch (classify(ox, oy, dx, dy, ax, ay)) {
        case LEFT:
            if (ay > oy && ay <= dy)
                return CROSS_LEFT;
            else
                return INESSENTIAL;
        case RIGHT:
            if (ay > dy && ay <= oy)
                return CROSS_RIGHT;
            else
                return INESSENTIAL;
        case BETWEEN:
        case ORIGIN:
        case DESTINATION:
            return TOUCHING;
        default:
            return INESSENTIAL;
    }
}

PType pInPolygonEOMode(double x, double y, double vertices[][2],
                       int numVertices = 4) {
    int param = 0;

    for (int i = 0; i < numVertices; i++) {
        switch (edgeType(vertices[i][0], vertices[i][1],
                         vertices[(i + 1) % numVertices][0],
                         vertices[(i + 1) % numVertices][1], x, y)) {
            case TOUCHING:
                return INSIDE;
            case CROSS_LEFT:
            case CROSS_RIGHT:
                param = 1 - param;
        }
    }

    if (param == 1)
        return INSIDE;
    else
        return OUTSIDE;
}

void fill(cv::Mat& image, double vertices[4][2], int scale,
          const cv::Vec3b& color, double minX, double maxY) {
    double min_x = vertices[0][0], min_y = vertices[0][1];
    double max_x = vertices[0][0], max_y = vertices[0][1];

    for (int i = 1; i < 4; ++i) {
        min_x = std::min(min_x, vertices[i][0]);
        min_y = std::min(min_y, vertices[i][1]);
        max_x = std::max(max_x, vertices[i][0]);
        max_y = std::max(max_y, vertices[i][1]);
    }

    double step = 1. / (2 * scale);

    for (double x = min_x; x <= max_x; x += step) {
        for (double y = min_y; y <= max_y; y += step) {
            PType pointType;
            pointType = pInPolygonEOMode(x, y, vertices);
            if (pointType == INSIDE) {
                int xInt, yInt;
                transformCoordinates(x, y, xInt, yInt, image.cols, image.rows,
                                     scale, minX, maxY);
                set_pixel(image, xInt, yInt, color);
            }
        }
    }
}
