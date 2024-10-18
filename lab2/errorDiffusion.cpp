#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>

#include "PngProc.h"

void errorDiffusion(unsigned char* pIn, unsigned char* pOut, size_t nWidth,
                    size_t nHeight, unsigned int nBB);

void errorDiffusionReverse(unsigned char* pIn, unsigned char* pOut,
                           size_t nWidth, size_t nHeight, unsigned int nBB);

void errorDiffusionStucki(unsigned char* pIn, unsigned char* pOut,
                          size_t nWidth, size_t nHeight, unsigned int nBB);

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

    int n;
    printf("Enter new number of bits per pixel\n");
    std::cin >> n;

    if (((n & (n - 1)) != 0) || (n > 8)) {
        printf(
            "Incorrect number of n. It should be positive, less than 8 and "
            "power of "
            "2 \n");
        return -1;
    }

    int type;
    printf(
        "Enter mode of diffusion:\n"
        "1: Floyd-Steinberg\n"
        "2: Reverse for some rows\n"
        "3: Stucki\n");
    std::cin >> type;

    if (type > 3 || type < 1) {
        printf("Incorrect type. It should be 1-3.");
        return -1;
    }

    // parse input parameters
    char szInputFileName[256];
    char szOutputFileName[256];
    if (argc < 2)
        printf("\nformat: pngtest <input_file>");
    else {
        strcpy(szInputFileName, argv[1]);
        strcpy(szOutputFileName, szInputFileName);
        strcat(szOutputFileName, std::to_string(n).c_str());
        strcat(szOutputFileName, "_type");
        strcat(szOutputFileName, std::to_string(type).c_str());
        strcat(szOutputFileName, "_out.png");
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

    unsigned char* pOutputBits = new unsigned char[nReqSize];
    if (!pOutputBits) {
        printf("\nCan't allocate memory for image, required size is %lu",
               nReqSize);
        delete[] pInputBits;
        pInputBits = nullptr;
        return -1;
    }

    CBitsPtrGuard OutputBitsPtrGuard(&pOutputBits);

    size_t nWidth, nHeight;

    size_t nRetSize = NPngProc::readPngFileGray(szInputFileName, pInputBits,
                                                &nWidth, &nHeight);

    switch (type) {
        case 1:
            errorDiffusion(pInputBits, pOutputBits, nWidth, nHeight, n);
            break;
        case 2:
            errorDiffusionReverse(pInputBits, pOutputBits, nWidth, nHeight, n);
            break;
        case 3:
            errorDiffusionStucki(pInputBits, pOutputBits, nWidth, nHeight, n);
            break;
        default:
            errorDiffusion(pInputBits, pOutputBits, nWidth, nHeight, n);
            break;
    }
    errorDiffusion(pInputBits, pOutputBits, nWidth, nHeight, n);

    if (NPngProc::writePngFile(szOutputFileName, pOutputBits, nWidth, nHeight,
                               8) == NPngProc::PNG_ERROR) {
        printf("\nError ocuured during png file was written");
        delete[] pInputBits;
        pInputBits = nullptr;
        delete[] pOutputBits;
        pOutputBits = nullptr;

        return -1;
    }

    delete[] pInputBits;
    pInputBits = nullptr;
    delete[] pOutputBits;
    pOutputBits = nullptr;

    return 0;
}

void errorDiffusion(unsigned char* pIn, unsigned char* pOut, size_t nWidth,
                    size_t nHeight, unsigned int nBB) {
    int maxValue = (1 << nBB) - 1;
    float scale = 255.0 / maxValue;

    for (size_t y = 0; y < nHeight; y++) {
        for (size_t x = 0; x < nWidth; x++) {
            int idx = x + y * nWidth;

            unsigned char oldValue = pIn[idx];
            unsigned char newValue =
                static_cast<unsigned char>(round(oldValue / scale) * scale);

            pOut[idx] = newValue;

            int error = oldValue - newValue;

            if (x + 1 < nWidth) {
                pIn[idx + 1] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + 1] + error * 7 / 16), 0, 255));
            }

            if (x > 0 && y + 1 < nHeight) {
                pIn[idx + nWidth - 1] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + nWidth - 1] + error * 3 / 16), 0,
                    255));
            }

            if (y + 1 < nHeight) {
                pIn[idx + nWidth] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + nWidth] + error * 5 / 16), 0,
                    255));
            }

            if (x + 1 < nWidth && y + 1 < nHeight) {
                pIn[idx + nWidth + 1] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + nWidth + 1] + error * 1 / 16), 0,
                    255));
            }
        }
    }
    return;
}

void errorDiffusionReverse(unsigned char* pIn, unsigned char* pOut,
                           size_t nWidth, size_t nHeight, unsigned int nBB) {
    int maxValue = (1 << nBB) - 1;
    float scale = 255.0 / maxValue;

    for (size_t y = 0; y < nHeight; y++) {
        if (y % 2 == 0) {
            for (size_t x = 0; x < nWidth; x++) {
                int idx = x + y * nWidth;

                unsigned char oldValue = pIn[idx];
                unsigned char newValue =
                    static_cast<unsigned char>(round(oldValue / scale) * scale);

                pOut[idx] = newValue;

                int error = oldValue - newValue;

                if (x + 1 < nWidth) {
                    pIn[idx + 1] = static_cast<unsigned char>(std::clamp(
                        static_cast<int>(pIn[idx + 1] + error * 7 / 16), 0,
                        255));
                }

                if (x > 0 && y + 1 < nHeight) {
                    pIn[idx + nWidth - 1] = static_cast<unsigned char>(
                        std::clamp(static_cast<int>(pIn[idx + nWidth - 1] +
                                                    error * 3 / 16),
                                   0, 255));
                }

                if (y + 1 < nHeight) {
                    pIn[idx + nWidth] = static_cast<unsigned char>(std::clamp(
                        static_cast<int>(pIn[idx + nWidth] + error * 5 / 16), 0,
                        255));
                }

                if (x + 1 < nWidth && y + 1 < nHeight) {
                    pIn[idx + nWidth + 1] = static_cast<unsigned char>(
                        std::clamp(static_cast<int>(pIn[idx + nWidth + 1] +
                                                    error * 1 / 16),
                                   0, 255));
                }
            }
        } else {
            for (size_t x = nWidth - 1; x - 1 > 0; x--) {
                int idx = x + y * nWidth;

                unsigned char oldValue = pIn[idx];
                unsigned char newValue =
                    static_cast<unsigned char>(round(oldValue / scale) * scale);

                pOut[idx] = newValue;

                int error = oldValue - newValue;

                if (x > 0) {
                    pIn[idx - 1] = static_cast<unsigned char>(std::clamp(
                        static_cast<int>(pIn[idx - 1] + error * 7 / 16), 0,
                        255));
                }

                if (x < nWidth - 1 && y + 1 < nHeight) {
                    pIn[idx + nWidth - 1] = static_cast<unsigned char>(
                        std::clamp(static_cast<int>(pIn[idx + nWidth - 1] +
                                                    error * 5 / 16),
                                   0, 255));
                }

                if (x < nWidth - 1 && y + 1 < nHeight) {
                    pIn[idx + nWidth] = static_cast<unsigned char>(std::clamp(
                        static_cast<int>(pIn[idx + nWidth] + error * 3 / 16), 0,
                        255));
                }

                if (x < nWidth - 1 && y + 1 < nHeight) {
                    pIn[idx + nWidth + 1] = static_cast<unsigned char>(
                        std::clamp(static_cast<int>(pIn[idx + nWidth + 1] +
                                                    error * 1 / 16),
                                   0, 255));
                }
            }
        }
    }
    return;
}

void errorDiffusionStucki(unsigned char* pIn, unsigned char* pOut,
                          size_t nWidth, size_t nHeight, unsigned int nBB) {
    int maxValue = (1 << nBB) - 1;
    float scale = 255.0 / maxValue;

    for (size_t y = 0; y < nHeight; y++) {
        for (size_t x = 0; x < nWidth; x++) {
            int idx = x + y * nWidth;

            unsigned char oldValue = pIn[idx];
            unsigned char newValue =
                static_cast<unsigned char>(round(oldValue / scale) * scale);

            pOut[idx] = newValue;

            int error = oldValue - newValue;

            if (x + 1 < nWidth) {
                pIn[idx + 1] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + 1] + error * 8 / 42), 0, 255));
            }

            if (x + 2 < nWidth) {
                pIn[idx + 2] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + 2] + error * 4 / 42), 0, 255));
            }

            if (x > 1 && y + 1 < nHeight) {
                pIn[idx + nWidth - 2] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + nWidth - 2] + error * 2 / 42), 0,
                    255));
            }

            if (x > 0 && y + 1 < nHeight) {
                pIn[idx + nWidth - 1] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + nWidth - 1] + error * 4 / 42), 0,
                    255));
            }

            if (y + 1 < nHeight) {
                pIn[idx + nWidth] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + nWidth] + error * 8 / 42), 0,
                    255));
            }

            if (x + 1 < nWidth && y + 1 < nHeight) {
                pIn[idx + nWidth + 1] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + nWidth + 1] + error * 4 / 42), 0,
                    255));
            }

            if (x + 2 < nWidth && y + 1 < nHeight) {
                pIn[idx + nWidth + 2] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + nWidth + 2] + error * 2 / 42), 0,
                    255));
            }

            if (x > 1 && y + 2 < nHeight) {
                pIn[idx + 2 * nWidth - 2] = static_cast<unsigned char>(
                    std::clamp(static_cast<int>(pIn[idx + 2 * nWidth - 2] +
                                                error * 1 / 42),
                               0, 255));
            }

            if (x > 0 && y + 2 < nHeight) {
                pIn[idx + 2 * nWidth - 1] = static_cast<unsigned char>(
                    std::clamp(static_cast<int>(pIn[idx + 2 * nWidth - 1] +
                                                error * 2 / 42),
                               0, 255));
            }

            if (y + 2 < nHeight) {
                pIn[idx + 2 * nWidth] = static_cast<unsigned char>(std::clamp(
                    static_cast<int>(pIn[idx + 2 * nWidth] + error * 4 / 42), 0,
                    255));
            }

            if (x + 1 < nWidth && y + 2 < nHeight) {
                pIn[idx + 2 * nWidth + 1] = static_cast<unsigned char>(
                    std::clamp(static_cast<int>(pIn[idx + 2 * nWidth + 1] +
                                                error * 2 / 42),
                               0, 255));
            }

            if (x + 2 < nWidth && y + 2 < nHeight) {
                pIn[idx + 2 * nWidth + 2] = static_cast<unsigned char>(
                    std::clamp(static_cast<int>(pIn[idx + 2 * nWidth + 2] +
                                                error * 1 / 42),
                               0, 255));
            }
        }
    }
    return;
}