#include "utils.h"

#include "checkIntersections.h"

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

    drawLine(image, spacing, 0, spacing, height, black);
    drawLine(image, 0, height - spacing, width, height - spacing, black);
}

void transformCoordinates(double x, double y, int& xOut, int& yOut,
                          int windowWidth, int windowHeight, int scale) {
    xOut = static_cast<int>(std::round(1 * scale + x * scale));
    yOut = static_cast<int>(std::round(windowHeight - scale - y * scale));
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

int checkOrientation(double vertices[][2]) {
    int initialSide = 0;
    double signedArea = 0.0;

    double x1 = vertices[0][0];
    double y1 = vertices[0][1];
    double x2 = vertices[1][0];
    double y2 = vertices[1][1];

    double nextX = vertices[2][0];
    double nextY = vertices[2][1];

    CLPointType position = classify(x1, y1, x2, y2, nextX, nextY);

    int currentSide = (position == LEFT) ? 0 : 1;

    return currentSide;
}

bool isPointOnSegment(double x1, double y1, double x2, double y2, double px,
                      double py) {
    CLPointType pos = classify(x1, y1, x2, y2, px, py);
    if (pos == BETWEEN || pos == ORIGIN || pos == DESTINATION) {
        return true;
    }
    return false;
}
