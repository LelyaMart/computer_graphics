// pngtest.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string.h>

#include <cstddef>
#include <iostream>

#include "PngProc.h"

void AcceptCircleMask(unsigned char* pImage, size_t nWidth, size_t nHeight);

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

    // parse input parameters
    char szInputFileName[256];
    char szOutputFileName[256];
    if (argc < 2)
        printf("\nformat: drawCircle <input_file> [<output_file>]");
    else {
        strcpy(szInputFileName, argv[1]);
        if (argc > 2)
            strcpy(szOutputFileName, argv[2]);
        else {
            strcpy(szOutputFileName, szInputFileName);
            strcat(szOutputFileName, "_out.png");
        }
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
    unsigned int nBPP;

    size_t nRetSize = NPngProc::readPngFileGray(szInputFileName, pInputBits,
                                                &nWidth, &nHeight /*, &nBPP*/);
    nBPP = 8;

    AcceptCircleMask(pInputBits, nWidth, nHeight);

    if (NPngProc::writePngFile(szOutputFileName, pInputBits, nWidth, nHeight,
                               nBPP) == NPngProc::PNG_ERROR) {
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

void AcceptCircleMask(unsigned char* pImage, size_t nWidth, size_t nHeight) {
    int diametr = std::min(nWidth, nHeight);
    int radius = diametr / 2;
    int centerX = nWidth / 2;
    int centerY = nHeight / 2;

    for (int y = 0; y < nHeight; y++) {
        for (int x = 0; x < nWidth; x++) {
            int dx = x - centerX;
            int dy = y - centerY;
            if (dx * dx + dy * dy > radius * radius) {
                *pImage = 255;
            }
            pImage++;
        }
    }
    return;
}
