#pragma once
#include"vec3.h"

class Boundary {
	public:
		Boundary(float x1, float y1, float z1, float x2, float y2, float z2);
		vec3 a;
		vec3 b;
};
