#include "checkIntersections.h"

CLPointType classify(double x1, double y1, double x2, double y2, double x,
                     double y) {
    double ax = x2 - x1;
    double ay = y2 - y1;

    double bx = x - x1;
    double by = y - y1;

    double s = ax * by - bx * ay;

    if (s > 0) return LEFT;
    if (s < 0) return RIGHT;

    if ((ax * bx < 0) || (ay * by < 0)) return BEHIND;
    if ((ax * ax + ay * ay) < (bx * bx + by * by)) return BEYOND;

    if (x1 == x && y1 == y) return ORIGIN;
    if (x2 == x && y2 == y) return DESTINATION;

    return BETWEEN;
}

IntersectType intersect(double ax, double ay, double bx, double by, double cx,
                        double cy, double dx, double dy, double *t) {
    double nx = dy - cy;
    double ny = cx - dx;
    CLPointType type;

    double denom = nx * (bx - ax) + ny * (by - ay);
    if (denom == 0) {
        type = classify(cx, cy, dx, dy, ax, ay);
        if (type == LEFT || type == RIGHT)
            return PARALLEL;
        else
            return SAME;
    }
    double num = nx * (ax - cx) + ny * (ay - cy);
    *t = -num / denom;
    return SKEW;
}

IntersectType cross(double ax, double ay, double bx, double by, double cx,
                    double cy, double dx, double dy, double *tab, double *tcd) {
    IntersectType type = intersect(ax, ay, bx, by, cx, cy, dx, dy, tab);
    if (type == SAME || type == PARALLEL) return type;
    if ((*tab < 0) || (*tab > 1)) return SKEW_NO_CROSS;
    intersect(cx, cy, dx, dy, ax, ay, bx, by, tcd);
    if ((*tcd < 0) || (*tcd > 1)) return SKEW_NO_CROSS;
    return SKEW_CROSS;
}
