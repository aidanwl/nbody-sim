#include "core/vec2.h"

// Adds two 2D vectors component-by-component.
Vector2 vec2_vadd(Vector2 a, Vector2 b) {
	return (Vector2){a.x + b.x, a.y + b.y};
}

// Subtracts b from a component-by-component.
Vector2 vec2_vsub(Vector2 a, Vector2 b) {
	return (Vector2){a.x - b.x, a.y - b.y};
}

// Scales a 2D vector by a scalar value.
Vector2 vec2_vscale(Vector2 v, float s) {
	return (Vector2){v.x * s, v.y * s};
}
