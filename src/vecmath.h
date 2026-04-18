#ifndef VECMATH_H
#define VECMATH_H
#include <math.h>
#include <stdio.h>
#define G 1.0
#define EPSILON 10e-10

typedef struct {
    double x;
    double y;
} Vector2;

typedef struct {
    double mass;
    Vector2 position;
    Vector2 velocity;
    Vector2 force; 
} Body;

Vector2 vec_add(Vector2 a, Vector2 b);
Vector2 vec_sub(Vector2 a, Vector2 b);
Vector2 vec_scale(Vector2 v, double s);
double norm(Vector2 v);
Vector2 vec_norm(Vector2 v);
double distance(Vector2 a, Vector2 b);

double force_mag(Body *a, Body *b);
Vector2 vec_force(Body *a, Body *b);

#endif


