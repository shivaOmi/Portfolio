#ifndef HITTABLEH
#define HITTABLEH

#include"ray.h"

class material;

struct hit_record{
    float t;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

class hittable{
    public:
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec)const = 0;
};

float hit_sphere(const vec3& center, float radius, const ray& r){
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0f * dot(oc, r.direction());
    float c = dot(oc,oc) - radius*radius;
    float discriminant = b*b - 4*a*c;

    if(discriminant < 0){
        return -1.0f;
    }
    else{
        return ((-b + sqrt(discriminant))/(2.0f*a));
    }
}

#endif