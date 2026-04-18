#include <stdio.h>
#include <math.h>
#include "vecmath.h"

Vector2 vec_add(Vector2 vec1, Vector2 vec2){

	Vector2 sum;
	sum.x = vec1.x + vec2.x;
	sum.y = vec1.y + vec2.y;

	return sum; 
}

Vector2 vec_sub(Vector2 vec1, Vector2 vec2){

	Vector2 sub;
	sub.x = vec1.x - vec2.x;
	sub.y = vec1.y - vec2.y;

	return sub; 

}

Vector2 vec_scale(Vector2 vec, double s){

	Vector2 scl;
	scl.x = s * vec.x;
	scl.y = s * vec.y;

	return scl;

}

double norm(Vector2 vec){

	return hypot(vec.x, vec.y);

}

Vector2 vec_norm(Vector2 vec){
	
	double n = norm(vec);

	if(n < EPSILON){
	return (Vector2){0.0, 0.0};
	}
	
	return vec_scale(vec, 1.0 / n);
}

double distance(Vector2 vec1, Vector2 vec2){

	return norm(vec_sub(vec1, vec2));

}

double force_mag(Body *a, Body *b){

	return (a->mass * b->mass * G) / (pow(distance(a->position, b->position), 2) + EPSILON * EPSILON);

}

Vector2 vec_force(Body *a, Body *b){
	
	return vec_scale(vec_norm(vec_sub(b->position, a->position)), force_mag(a, b));  	

}




