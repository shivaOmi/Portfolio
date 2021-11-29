#pragma once
#include"vec3.h"
#include"Boundary.h"

class Ray{
    public:
        Ray(){}
        Ray(const vec3& a, const vec3& b){ A = a; B = b; }
        vec3 origin() const { return A; }
        vec3 direction() const { return B; }
        vec3 point_at_parameter(float t){ return A + t*B; }
        vec3 cast(Boundary* wall);

        vec3 A;
        vec3 B;
};