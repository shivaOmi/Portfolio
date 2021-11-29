#include"Boundary.h"

Boundary::Boundary(float x1, float y1, float z1, float x2, float y2, float z2) {
	this->a = vec3(x1, y1, z1);
	this->b = vec3(x2, y2, z2);
}