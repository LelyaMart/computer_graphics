#include <stdio.h>
#include <string.h>

#include <cstddef>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "PngProc.h"

void rgbToCmyk(const unsigned char* pBuf, size_t width, size_t height, float q,
               float* C, float* M, float* Y, float* K) {
    for (int i = 1; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int idx = (i * width + j) * 3;
            float r = pBuf[idx] / 255.0f;
            float g = pBuf[idx + 1] / 255.0f;
            float b = pBuf[idx + 2] / 255.0f;

            float k = q * std::min({1.0f - r, 1.0f - g, 1.0f - b});
            C[i * width + j] = (1.0f - r - k) / (1.0f - k + 1e-5f);
            M[i * width + j] = (1.0f - g - k) / (1.0f - k + 1e-5f);
            Y[i * width + j] = (1.0f - b - k) / (1.0f - k + 1e-5f);
            K[i * width + j] = k;
        }
    }
}

void computeHistogram(const float* channel, size_t width, size_t height,
                      int* histogram, int bins = 256) {
    for (int i = 0; i < bins; ++i) {
        histogram[i] = 0;
    }

    for (size_t i = 0; i < width * height; ++i) {
        int bin = static_cast<int>(channel[i] * (bins - 1));
        histogram[bin]++;
    }
}

cv::Mat drawHistogram(const int* histogram, const cv::Scalar& color, int bins,
                      int height) {
    int max_value = 0;
    for (int i = 0; i < bins; ++i) {
        max_value = std::max(max_value, histogram[i]);
    }

    cv::Mat histImage(height, bins * 2, CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < bins; ++i) {
        int bin_height = cvRound((double)histogram[i] / max_value * height);
        cv::rectangle(histImage, cv::Point(i * 2, height - bin_height),
                      cv::Point((i + 1) * 2 - 1, height), color, cv::FILLED);
    }

    return histImage;
}

int main(int argc, char* argv[]) {
    class CBitsPtrGuard {
       public:
        CBitsPtrGuard(unsigned char** pB) : m_ppBits(pB) {}
        ~CBitsPtrGuard() {
            if (*m_ppBits) delete *m_ppBits, *m_ppBits = 0;
        }

       protected:
        unsigned char** m_ppBits;
    };

    float q;
    std::cout << "Enter q: ";
    std::cin >> q;

    if (q < 0 || q > 1) {
        std::cerr << "q should be positive and less than 1";
        return -1;
    }

    char szInputFileName[256];
    if (argc < 2)
        printf("\nformat: pngtest <input_file>");
    else {
        strcpy(szInputFileName, argv[1]);
    }

    size_t nReqSize = NPngProc::readPngFile(szInputFileName, 0, 0, 0, 0);
    if (nReqSize == NPngProc::PNG_ERROR) {
        printf("\nError ocured while pngfile was read");
        return -1;
    }

    unsigned char* pInputBits = new unsigned char[nReqSize];
    if (!pInputBits) {
        printf("\nCan't allocate memory for image, required size is %lu",
               nReqSize);
        return -1;
    }
    CBitsPtrGuard InputBitsPtrGuard(&pInputBits);

    size_t width, height;

    unsigned int nBPP = 24;

    size_t nRetSize = NPngProc::readPngFile(szInputFileName, pInputBits, &width,
                                            &height, &nBPP);

    float* C = new float[width * height];
    float* M = new float[width * height];
    float* Y = new float[width * height];
    float* K = new float[width * height];

    rgbToCmyk(pInputBits, width, height, q, C, M, Y, K);

    delete[] pInputBits;
    pInputBits = nullptr;

    int histC[256], histM[256], histY[256], histK[256];
    computeHistogram(C, width, height, histC);
    computeHistogram(M, width, height, histM);
    computeHistogram(Y, width, height, histY);
    computeHistogram(K, width, height, histK);

    int histHeight = 400;
    int bins = 256;

    cv::Mat histCImg =
        drawHistogram(histC, cv::Scalar(255, 255, 0), bins, histHeight);
    cv::Mat histMImg =
        drawHistogram(histM, cv::Scalar(255, 0, 255), bins, histHeight);
    cv::Mat histYImg =
        drawHistogram(histY, cv::Scalar(0, 255, 255), bins, histHeight);
    cv::Mat histKImg =
        drawHistogram(histK, cv::Scalar(0, 0, 0), bins, histHeight);

    delete[] C;
    delete[] M;
    delete[] Y;
    delete[] K;

    int canvasWidth = bins * 4 + 5;
    int canvasHeight = histHeight * 2 + 5;
    cv::Mat canvas = cv::Mat::zeros(canvasHeight, canvasWidth, CV_8UC3);

    cv::Rect roiC(0, 0, bins * 2, histHeight);
    cv::Rect roiM(bins * 2 + 5, 0, bins * 2, histHeight);
    cv::Rect roiY(0, histHeight + 5, bins * 2, histHeight);
    cv::Rect roiK(bins * 2 + 5, histHeight + 5, bins * 2, histHeight);

    histCImg.copyTo(canvas(roiC));
    histMImg.copyTo(canvas(roiM));
    histYImg.copyTo(canvas(roiY));
    histKImg.copyTo(canvas(roiK));

    cv::imshow("CMYK Histograms", canvas);
    cv::waitKey(0);
    return 0;
}
