#ifndef PAINT_CLASSIFICATION_H
#define PAINT_CLASSIFICATION_H

enum EType { TOUCHING, CROSS_LEFT, CROSS_RIGHT, INESSENTIAL };

enum PType { INSIDE, OUTSIDE };

EType edgeType(double ox, double oy, double dx, double dy, double ax,
               double ay);

PType pInPolygonEOMode(double x, double y, double vertices[][2],
                       int numVertices);

PType pInPolygonNZWMode(double x, double y, double vertices[][2],
                        int numVertices);

#endif  // PAINT_CLASSIFICATION_H
