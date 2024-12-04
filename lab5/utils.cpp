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
