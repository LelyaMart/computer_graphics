## Лабораторная работа 4: Кривые Безье, отсечение отрезков прямых

Реализовать следующие функции:
1. Построения кривых Безье третьего порядка.
2. Отсечения отрезков прямых выпуклым полигоном с помощью алгоритма
Кируса-Бека.

First task compiling: g++ bezierCurves.cpp utils.cpp checkIntersections.cpp -o bezierCurves `pkg-config --cflags --libs opencv4`

Second task compiling: g++ clipLine.cpp utils.cpp checkIntersections.cpp -o clipLine `pkg-config --cflags --libs opencv4`
