## Лабораторная работа 3. Построение и заполнение полигонов:

Реализовать следующие функции:
1. Вычерчивания отрезков прямых линий толщиной в 1 пиксел.
2. Вывода на экран полигона.
3. Определения типа полигона: простой или сложный (т.е. с самопересечениями),
выпуклый или невыпуклый.
4. Заполнение полигона, используя правила even-odd и non-zero-winding определения принадлежности полигону.

Основной файл - drawPolygon.cpp.

Результаты выполнения приведены в папке pictures.

Определение типа многоугольника выводится в консоль, а также показаны цветами изображений:

* Если многоугольник выпуклый и без самопересечений, то он *зеленый*

* Если без самопересечений, но не выпуклый - *синий*

* Если есть самопересечения - *красный*.


Compiling: g++ drawPolygon.cpp drawLine.cpp checkIntersections.cpp paintClassification.cpp -o draw `pkg-config --cflags --libs opencv4`