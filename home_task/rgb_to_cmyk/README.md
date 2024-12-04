Напишите программу, которая для цветного изображения 24 bpp, которое изначально в
RGB, строит гистограммы цветовых компонент C, M, Y, K. Для преобразования RGB в
CMYK используется алгоритм q% Gray Component Replacement (GCR). Значение q задаётся
пользователем.

Compiling: g++ cmykHystogram.cpp PngProc.cpp -o hist -lpng `pkg-config --cflags --libs opencv4`

Usage: ./hist <image.png>