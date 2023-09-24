#include <stdio.h>
#include <stdbool.h>
#include "dmath.h"
#include <math.h>

// see: https://en.wikipedia.org/wiki/Line–line_intersection
// compute intersection of two line segments, returns (NAN, NAN) if there is
// no intersection.
v2 intersect_segs(v2 a0, v2 a1, v2 b0, v2 b1) {
    const float d =
        ((a0.x - a1.x) * (b0.y - b1.y))
        - ((a0.y - a1.y) * (b0.x - b1.x));

    if (qabs(d) < 0.000001f) { return (v2) { NAN, NAN }; }

    const float
        t = (((a0.x - b0.x) * (b0.y - b1.y))
            - ((a0.y - b0.y) * (b0.x - b1.x))) / d,
        u = (((a0.x - b0.x) * (a0.y - a1.y))
            - ((a0.y - b0.y) * (a0.x - a1.x))) / d;
    return (t >= 0 && t <= 1 && u >= 0 && u <= 1) ?
        ((v2) {
        a0.x + (t * (a1.x - a0.x)),
            a0.y + (t * (a1.y - a0.y))
    })
        : ((v2) {
        NAN, NAN
    });
}

float angle_between_points(v2 a, v2 b) {
    /*                (a.x-b.x)
    * arccos(-----------------------------)
    *        sqrt((a.x-b.x)^2+(b.y-a.y)^2)
    */
    if (b.y - a.y == 0) return 0;
    return ((b.y - a.y) / qabs(b.y - a.y)) *
        acosf(
            (a.x - b.x) /
            (sqrtf(square(a.x - b.x) + square(b.y - a.y)))
        );

}

// https://gamefromscratch.com/gamedev-math-recipes-rotating-one-point-around-another-point/
v2 rotate_about(v2 a, v2 center, float angle) {
    float rotatedX = cosf(angle) * (a.x - center.x) - sinf(angle) * (a.y - center.y) + center.x;
    float rotatedY = sinf(angle) * (a.x - center.x) + cosf(angle) * (a.y - center.y) + center.y;

    return (v2) { rotatedX, rotatedY };
}