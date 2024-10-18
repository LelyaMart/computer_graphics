#include <stdio.h>
#include <string.h>

#include <cstddef>
#include <iostream>

#include "PngProc.h"

void blendImages(unsigned char* pOut, unsigned char* pIn1, unsigned char* pIn2,
                 unsigned char* pAlpha, size_t nWidth, size_t nHeight,
                 int blendMode);

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

    if (argc != 4) {
        printf("Usage: %s <input1.png> <input2.png> <input3.png>", argv[0]);
        return 1;
    }

    size_t nWidth1, nHeight1, nWidth2, nHeight2, nWidthAlpha, nHeightAlpha;

    size_t nReqSize1 = NPngProc::readPngFile(argv[1], 0, 0, 0, 0);
    if (nReqSize1 == NPngProc::PNG_ERROR) {
        printf("Error occurred while reading PNG file");
        return -1;
    }

    unsigned char* pInputBits1 = new unsigned char[nReqSize1];
    if (!pInputBits1) {
        printf("Can't allocate memory");
        return -1;
    }
    CBitsPtrGuard InputBitsPtrGuard1(&pInputBits1);

    if (NPngProc::readPngFileGray(argv[1], pInputBits1, &nWidth1, &nHeight1) ==
        NPngProc::PNG_ERROR) {
        printf("Error reading image data");
        delete[] pInputBits1;
        pInputBits1 = nullptr;

        return -1;
    }

    size_t nReqSize2 = NPngProc::readPngFile(argv[2], 0, 0, 0, 0);
    if (nReqSize2 == NPngProc::PNG_ERROR) {
        printf("Error occurred while reading PNG file");
        delete[] pInputBits1;
        pInputBits1 = nullptr;
        return -1;
    }

    unsigned char* pInputBits2 = new unsigned char[nReqSize2];
    if (!pInputBits2) {
        printf("Can't allocate memory");
        delete[] pInputBits1;
        pInputBits1 = nullptr;
        return -1;
    }
    CBitsPtrGuard InputBitsPtrGuard2(&pInputBits2);

    if (NPngProc::readPngFileGray(argv[2], pInputBits2, &nWidth2, &nHeight2) ==
        NPngProc::PNG_ERROR) {
        printf("Error reading image data");
        delete[] pInputBits1;
        pInputBits1 = nullptr;
        delete[] pInputBits2;
        pInputBits2 = nullptr;
        return -1;
    }

    size_t nReqSize3 = NPngProc::readPngFile(argv[3], 0, 0, 0, 0);
    if (nReqSize3 == NPngProc::PNG_ERROR) {
        printf("Error occurred while reading PNG file");
        delete[] pInputBits1;
        pInputBits1 = nullptr;
        delete[] pInputBits2;
        pInputBits2 = nullptr;
        return -1;
    }

    unsigned char* pInputBits3 = new unsigned char[nReqSize3];
    if (!pInputBits3) {
        printf("Can't allocate memory");
        delete[] pInputBits1;
        pInputBits1 = nullptr;
        delete[] pInputBits2;
        pInputBits2 = nullptr;
        return -1;
    }
    CBitsPtrGuard InputBitsPtrGuard3(&pInputBits3);

    if (NPngProc::readPngFileGray(argv[3], pInputBits3, &nWidthAlpha,
                                  &nHeightAlpha) == NPngProc::PNG_ERROR) {
        printf("Error reading image data");
        delete[] pInputBits1;
        pInputBits1 = nullptr;
        delete[] pInputBits2;
        pInputBits2 = nullptr;
        delete[] pInputBits3;
        pInputBits3 = nullptr;
        return -1;
    }

    if (nWidth1 != nWidth2 || nWidth1 != nWidthAlpha || nHeight1 != nHeight2 ||
        nHeight2 != nHeightAlpha) {
        printf("Input images must have the same dimensions");
        delete[] pInputBits1;
        pInputBits1 = nullptr;
        delete[] pInputBits2;
        pInputBits2 = nullptr;
        delete[] pInputBits3;
        pInputBits3 = nullptr;
        return -1;
    }

    unsigned char* pOutputBits = new unsigned char[nReqSize1];
    if (!pOutputBits) {
        printf("\nCan't allocate memory for image, required size is %lu",
               nReqSize1);
        delete[] pInputBits1;
        pInputBits1 = nullptr;
        delete[] pInputBits2;
        pInputBits2 = nullptr;
        delete[] pInputBits3;
        pInputBits3 = nullptr;
        return -1;
    }

    CBitsPtrGuard OutputBitsPtrGuard(&pOutputBits);

    int blendMode;
    printf(
        "Enter blend mode:\n0: Normal,\n1: Multiply,\n2: Screen,\n3: "
        "Darken,\n4:Lighten,\n5: Difference\n");
    std::cin >> blendMode;

    unsigned int nBPP = 8;

    blendImages(pOutputBits, pInputBits1, pInputBits2, pInputBits3, nWidth1,
                nHeight1, blendMode);

    std::string outputFileName =
        std::string(argv[1]) + "_" + std::string(argv[2]) + "_" +
        std::string(argv[3]) + std::to_string(blendMode) + "_res.png";

    if (NPngProc::writePngFile(outputFileName.c_str(), pOutputBits, nWidth1,
                               nHeight1, nBPP) == NPngProc::PNG_ERROR) {
        printf("\nError ocuured during png file was written");
        delete[] pInputBits1;
        pInputBits1 = nullptr;
        delete[] pInputBits2;
        pInputBits2 = nullptr;
        delete[] pInputBits3;
        pInputBits3 = nullptr;

        delete[] pOutputBits;
        pOutputBits = nullptr;

        return -1;
    }
    delete[] pInputBits1;
    pInputBits1 = nullptr;
    delete[] pInputBits2;
    pInputBits2 = nullptr;
    delete[] pInputBits3;
    pInputBits3 = nullptr;

    delete[] pOutputBits;
    pOutputBits = nullptr;

    return 0;
}

void blendImages(unsigned char* pOut, unsigned char* pIn1, unsigned char* pIn2,
                 unsigned char* pAlpha, size_t nWidth, size_t nHeight,
                 int blendMode) {
    for (int y = 0; y < nHeight; y++) {
        for (int x = 0; x < nWidth; x++) {
            size_t idx = x + y * nWidth;
            float alpha_s = pAlpha[idx] / 255.0f;
            float alpha_b = 1 - alpha_s;

            unsigned char C_s = pIn1[idx];
            unsigned char C_b = pIn2[idx];

            unsigned char B = 0.0f;
            switch (blendMode) {
                case 0:
                    B = C_s;
                    break;
                case 1:
                    B = (unsigned char)(C_b * C_s / 255);
                    break;
                case 2:
                    B = (unsigned char)((1 - (1 - C_b / 255.0f) *
                                                 (1 - C_s / 255.0f)) *
                                        255);
                    break;
                case 3:
                    B = std::min(C_b, C_s);
                    break;
                case 4:
                    B = std::max(C_b, C_s);
                    break;
                case 5:
                    B = abs(C_b - C_s);
                    break;
            }
            float C_r = ((1 - alpha_s) * alpha_b * C_b) +
                        ((1 - alpha_b) * alpha_s * C_s) +
                        (alpha_b * alpha_s * B);

            pOut[idx] = static_cast<unsigned char>(C_r);
        }
    }
}
