#include "paintClassification.h"

#include <iostream>

#include "checkIntersections.h"

EType edgeType(double ox, double oy, double dx, double dy, double ax,
               double ay) {
    switch (classify(ox, oy, dx, dy, ax, ay)) {
        case LEFT:
            if (ay > oy && ay <= dy)
                return CROSS_LEFT;
            else
                return INESSENTIAL;
        case RIGHT:
            if (ay > dy && ay <= oy)
                return CROSS_RIGHT;
            else
                return INESSENTIAL;
        case BETWEEN:
        case ORIGIN:
        case DESTINATION:
            return TOUCHING;
        default:
            return INESSENTIAL;
    }
}

PType pInPolygonEOMode(double x, double y, double vertices[][2],
                       int numVertices) {
    int param = 0;

    for (int i = 0; i < numVertices; i++) {
        switch (edgeType(vertices[i][0], vertices[i][1],
                         vertices[(i + 1) % numVertices][0],
                         vertices[(i + 1) % numVertices][1], x, y)) {
            case TOUCHING:
                return INSIDE;
            case CROSS_LEFT:
            case CROSS_RIGHT:
                param = 1 - param;
        }
    }

    if (param == 1)
        return INSIDE;
    else
        return OUTSIDE;
}

PType pInPolygonNZWMode(double x, double y, double vertices[][2],
                        int numVertices) {
    int param = 0;

    for (int i = 0; i < numVertices; i++) {
        switch (edgeType(vertices[i][0], vertices[i][1],
                         vertices[(i + 1) % numVertices][0],
                         vertices[(i + 1) % numVertices][1], x, y)) {
            case TOUCHING:
                return INSIDE;
            case CROSS_LEFT:
                param++;
                break;
            case CROSS_RIGHT:
                param--;
                break;
        }
    }
    if (param != 0) {
        return INSIDE;
    } else
        return OUTSIDE;
}