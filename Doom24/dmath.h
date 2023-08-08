#pragma once
#include <stdint.h>
#include <math.h>
#include <tgmath.h>

typedef struct v2 { float x, y; } v2;
typedef struct v2i { int32_t x, y; } v2i;

#define dot(v0, v1) ((v0.x * v1.x) + (v0.y * v1.y))
#define length(v) (sqrtf(dot(v, v)))
#define seglength(v0, v1) (sqrt((v0.x - v1.x) * (v0.x - v1.x) + (v0.y - v1.y) * (v0.y - v1.y)))
#define min(a,b) ((a > b) ? (b) : (a))
#define max(a,b) ((a > b) ? (a) : (b))
#define sign(a)  ((a > 0) ? (1) : (-1))
#define v2_to_v2i(v) ((v2i) { v.x, v.y })
//#define v2i_to_v2(v) ((v2)  { v.x, v.y })
#define square(v) ((v)*(v))
#define normalize_v(u) ((v2) { u.x / length(u), u.y / length(u) })
#define compare_v2(a) ((a.x == a.x && a.y == a.y) ? true : false)
#define slope(a, b) ((b.y-a.y)/(b.x-a.x))
#define normalize_a(a) (atan2f(sinf(a), cosf(a)))
#define PI 3.14159265359f
#define TAU (2.0f * PI)
#define PI_2 (PI / 2.0f)
#define PI_4 (PI / 4.0f)

// Mathematical approximations
#define qabs(v) (((v) >= 0) ? (v) : -(v))
#define qcos(v) (qsin(PI_2-(v)))
#define qarcsin(v) (1.05938*v - 0.235601*v*v*v + 0.613381*v*v*v*v*v)
//#define qarccos(v) (-1.05938*v + 0.235601*v*v*v - 0.613381*v*v*v*v*v + 1.5708)
//#define qabs(v) (fabsf(v))
//#define qsin(v) (sin(v))
//#define qcos(v) (cos(v))
//#define qarccos(v) (acos(v))

float fast_acos(float a);
v2 intersect_segs(v2 a0, v2 a1, v2 b0, v2 b1);
float angle_between_points(v2 a, v2 b);
v2 rotate_about(v2 a, v2 center, float angle);
float fast_qsqrt(float number);
inline float qarccos(float v);
inline float qsin(float v);