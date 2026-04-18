#include <stdio.h>
#include <math.h>

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
	Vector2 result; 
	norm.x = vec.x / n;
	norm.y = vec.y / n; 
	return result;

}

Vector2 force 






