#include "drawLine.h"

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

// int main() {
//     cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);

//     cv::Vec3b red(0, 0, 255);

//     drawLine(image, 10, 10, 300, 200, red);

//     cv::imshow("Line", image);
//     cv::waitKey(0);
//     return 0;
// }