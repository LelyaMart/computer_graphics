#include <iostream>
#include <opencv2/opencv.hpp>

#include "utils.h"

void transformationMatrixFunc(int type, double transformationMatrix[4][4]) {
    if (type == 1) {
        double tempMatrix[4][4] = {
            {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 1}};
        std::copy(&tempMatrix[0][0], &tempMatrix[0][0] + 16,
                  &transformationMatrix[0][0]);
    } else if (type == 2) {
        double tempMatrix[4][4] = {{0.707, 0.408, 0, 0},
                                   {0, 0.816, 0, 0},
                                   {-0.707, 0.408, 0, 0},
                                   {0, 0, 0, 1}};
        std::copy(&tempMatrix[0][0], &tempMatrix[0][0] + 16,
                  &transformationMatrix[0][0]);
    } else if (type == 3) {
        double tempMatrix[4][4] = {
            {1, 0, 0, 0}, {0, 1, 0, 0}, {0.707, 0.707, 0, 0}, {0, 0, 0, 1}};
        std::copy(&tempMatrix[0][0], &tempMatrix[0][0] + 16,
                  &transformationMatrix[0][0]);
    }
}

void transformationPerspectiveMatrixFunc(double k,
                                         double transformationMatrix[4][4]) {
    double tempMatrix[4][4] = {
        {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 1 / k}, {0, 0, 0, 1}};
    std::copy(&tempMatrix[0][0], &tempMatrix[0][0] + 16,
              &transformationMatrix[0][0]);
}

void rotate(double& x, double& y, double& z, double axisX, double axisY,
            double axisZ, double angle) {
    normalize(axisX, axisY, axisZ);

    double cosA = std::cos(angle);
    double sinA = std::sin(angle);
    double dot = x * axisX + y * axisY + z * axisZ;

    double new_x, new_y, new_z;

    new_x =
        x * cosA + axisX * dot * (1 - cosA) + (-axisZ * y + axisY * z) * sinA;
    new_y =
        y * cosA + axisY * dot * (1 - cosA) + (axisZ * x - axisX * z) * sinA;
    new_z =
        z * cosA + axisZ * dot * (1 - cosA) + (-axisY * x + axisX * y) * sinA;

    x = new_x;
    y = new_y;
    z = new_z;
}

void animation(cv::Mat& image, double vertices[8][3],
               double projectionMatrix[4][4], double k, double axisX,
               double axisY, double axisZ, double xMin, double yMax, int scale,
               const cv::Vec3b& color, cv::VideoWriter video) {
    double angle = 0.0;

    for (int i = 0; i < 300; ++i) {
        image.setTo(cv::Scalar(255, 255, 255));

        double transformed[8][3];

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 3; j++) {
                transformed[i][j] = vertices[i][j];
            }
            rotate(transformed[i][0], transformed[i][1], transformed[i][2],
                   axisX, axisY, axisZ, angle);
        }

        double projectedVertices[8][2];
        transformVertices(transformed, projectionMatrix, projectedVertices);

        int transformedInt[8][2];
        for (int i = 0; i < 8; i++) {
            transformCoordinates(projectedVertices[i][0],
                                 projectedVertices[i][1], transformedInt[i][0],
                                 transformedInt[i][1], image.cols, image.rows,
                                 scale, xMin, yMax);
        }

        drawPolygon(image, transformedInt, color);

        cv::imshow("Projected polygon", image);
        video.write(image);

        angle += 0.05;

        if (angle > 2 * M_PI) {
            angle -= 2 * M_PI;
        }

        if (cv::waitKey(30) == 27) break;
    }
}

int main() {
    double vertices[8][3];
    std::cout << "Enter coordinates of vertices (x y z).\nFirt - 4 points of "
                 "the one face\nSecond - 4 points of the parallel face "
              << std::endl;
    for (int i = 0; i < 8; ++i) {
        std::cin >> vertices[i][0] >> vertices[i][1] >> vertices[i][2];
    }

    int projectionType;
    std::cout << "Choose projection type:\n1 - top view\n2 - isometric "
                 "projection\n3 - free projection\n4 - perspective "
                 "projection with center in (0, 0, k)\n";
    std::cin >> projectionType;

    double projectionMatrix[4][4];
    double k = 0;
    if (projectionType < 1 || projectionType > 4) {
        std::cerr << "Incorrect projection type";
        return -1;
    } else if (projectionType == 4) {
        std::cout << "Enter k: ";
        std::cin >> k;
        transformationPerspectiveMatrixFunc(k, projectionMatrix);
    } else {
        transformationMatrixFunc(projectionType, projectionMatrix);
    }

    double projectedVertices[8][2];
    transformVertices(vertices, projectionMatrix, projectedVertices);

    int widht, height;
    int scale = 100;
    double xMin, yMax;
    cv::Vec3b blue(255, 0, 0);

    calculateWindowSize(projectedVertices, widht, height, xMin, yMax);

    cv::Size windowSize = cv::Size(widht * scale, height * scale);

    cv::Mat image = cv::Mat(windowSize, CV_8UC3, cv::Scalar(255, 255, 255));

    int needAnimation;
    std::cout << "Shoud it be animated?\n0 - No\n1 - Yes\n";
    std::cin >> needAnimation;

    if (needAnimation) {
        double x, y, z;
        double fps = 30.0;

        std::cout << "Enter vector of rotation (x y z): ";
        std::cin >> x >> y >> z;
        cv::Size windowSize =
            cv::Size(widht * scale + 2 * scale, height * scale);
        cv::VideoWriter video(
            "./pictures/video3.avi",
            cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps,
            cv::Size(widht * scale + 2 * scale, height * scale));

        cv::Mat image = cv::Mat(windowSize, CV_8UC3, cv::Scalar(255, 255, 255));
        animation(image, vertices, projectionMatrix, k, x, y, z, xMin, yMax,
                  scale, blue, video);
        video.release();
        cv::destroyAllWindows();

    } else {
        int removeEdges;
        std::cout << "Should it to remove hidden edges?\n0 - No\n1 - Yes\n";
        std::cin >> removeEdges;
        if (!removeEdges) {
            int transformedVertices[8][2];
            for (int i = 0; i < 8; i++) {
                transformCoordinates(
                    projectedVertices[i][0], projectedVertices[i][1],
                    transformedVertices[i][0], transformedVertices[i][1], widht,
                    height, scale, xMin, yMax);
            }

            drawPolygon(image, transformedVertices, blue);

            cv::imshow("Projected polygon", image);
            // cv::imwrite("./pictures/picture3.png", image);
            cv::waitKey(0);
        } else {
            int faces[6][4] = {{0, 1, 2, 3}, {7, 5, 4, 3}, {0, 3, 7, 4},
                               {0, 4, 5, 1}, {1, 5, 6, 2}, {2, 6, 7, 3}};

            double viewerZ = -1.0f;

            for (int i = 0; i < 6; i++) {
                double faceVertices[4][2];
                for (int j = 0; j < 4; j++) {
                    faceVertices[j][0] = projectedVertices[faces[i][j]][0];
                    faceVertices[j][1] = projectedVertices[faces[i][j]][1];
                }
                double nz = computeNormal(faceVertices);

                if (nz * viewerZ >= 0) {
                    int transformedVertices[4][2];
                    for (int j = 0; j < 4; j++) {
                        transformCoordinates(projectedVertices[faces[i][j]][0],
                                             projectedVertices[faces[i][j]][1],
                                             transformedVertices[j][0],
                                             transformedVertices[j][1], widht,
                                             height, scale, xMin, yMax);
                    }
                    for (int j = 0; j < 4; j++) {
                        drawLine(image, transformedVertices[j][0],
                                 transformedVertices[j][1],
                                 transformedVertices[(j + 1) % 4][0],
                                 transformedVertices[(j + 1) % 4][1], blue);
                    }
                }
            }

            cv::imshow("Projected polygon", image);
            cv::imwrite("./pictures/picture5.png", image);
            cv::waitKey(0);
        }
    }
    return 0;
}