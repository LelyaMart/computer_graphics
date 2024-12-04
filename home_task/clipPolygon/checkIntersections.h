#ifndef CHECK_INTERSECTIONS_H
#define CHECK_INTERSECTIONS_H

enum CLPointType { LEFT, RIGHT, BEYOND, BEHIND, BETWEEN, ORIGIN, DESTINATION };

CLPointType classify(double x1, double y1, double x2, double y2, double x,
                     double y);

enum IntersectType { SAME, PARALLEL, SKEW, SKEW_CROSS, SKEW_NO_CROSS };

IntersectType intersect(double ax, double ay, double bx, double by, double cx,
                        double cy, double dx, double dy, double *t);

IntersectType cross(double ax, double ay, double bx, double by, double cx,
                    double cy, double dx, double dy, double *tab, double *tcd);

#endif  // CHECK_INTERSECTIONS_H
