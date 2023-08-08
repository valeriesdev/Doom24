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

inline float fast_sqrt(float v) {
    uint32_t i = *(uint32_t*)&v;
    i -= 1 << 23;
    i >>= 1;
    i += 1 << 29;
    float z = *(float*)&i;
    z = z - (z * z - v) / (2 * z); // Newton
    return z;
}

// quake III arena
float fast_qsqrt(float number) {
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long*)&y;                       // evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1);               // what the fuck?
    y = *(float*)&i;
    y = y * (threehalfs - (x2 * y * y));   // 1st iteration
   // y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y;
}

float angle_between_points(v2 a, v2 b) {
    /*                (a.x-b.x)
    * arccos(-----------------------------)
    *        sqrt((a.x-b.x)^2+(b.y-a.y)^2)
    */
    if (b.y - a.y == 0) return 0;
    return ((b.y - a.y) / qabs(b.y - a.y)) *
        acosf(
            (a.x - b.x) *
            (fast_qsqrt(square(a.x - b.x) + square(b.y - a.y)))
        );

}

inline float qarccos(float v) { // lol - still needs to be improved more
    return (v <= -0.981) ? (21865.5698156 * v + 66538.5189664 * v * v + 67487.0911944 * v * v * v + 22817.2584839 * v * v * v * v) : \
        (v >= 0.981) ? (21878.2623031 * v - 66557.7484966 * v * v + 67500.0391704 * v * v * v - 22820.5278244 * v * v * v * v) : \
        (-1.05938 * v + 0.235601 * v * v * v - 0.613381 * v * v * v * v * v + 1.5708);
}

inline float qsin(float v) {
    return (v - v * v * v / 6 + v * v * v * v * v / 120 - v * v * v * v * v * v * v / 5040 + v * v * v * v * v * v * v * v * v / 362880);
}


// https://gamefromscratch.com/gamedev-math-recipes-rotating-one-point-around-another-point/
v2 rotate_about(v2 a, v2 center, float angle) {
    float rotatedX = qcos(angle) * (a.x - center.x) - qsin(angle) * (a.y - center.y) + center.x;
    float rotatedY = qsin(angle) * (a.x - center.x) + qcos(angle) * (a.y - center.y) + center.y;

    return (v2) { rotatedX, rotatedY };
}