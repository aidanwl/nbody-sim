#ifndef VEC2_H
#define VEC2_H

typedef struct {
    double x;
    double y;
} Vector2;

// ---------- Vector Return ------------------------

Vector2 vec2_vadd(Vector2 a, Vector2 b);
Vector2 vec2_vsub(Vector2 a, Vector2 b);
Vector2 vec2_vscale(Vector2 v, double s);
Vector2 vec2_vnorm(Vector2 v);

// ---------- Scalar Return ------------------------

double vec2_snorm(Vector2 v);
double vec2_sdistance(Vector2 a, Vector2 b);

#endif