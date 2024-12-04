#include "utils.h"

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
