#include"ray.h"

vec3 Ray::cast(Boundary* wall) {
    float x1 = wall->a.e[0];
    float y_1 = wall->a.e[1];
    float x2 = wall->b.e[0];
    float y2 = wall->b.e[1];

    float x3 = this->A.e[0];
    float y3 = this->A.e[1];
    float x4 = (x3 + this->B.e[0]);
    float y4 = (y3 + this->B.e[1]);

    float den = (x1 - x2) * (y3 - y4) - (y_1 - y2) * (x3 - x4);

    if (den != 0) {
        float t = ((x1 - x3) * (y3 - y4) - (y_1 - y3) * (x3 - x4)) / den;
        float u = -(((x1 - x2) * (y_1 - y3) - (y_1 - y2) * (x1 - x3)) / den);

        if (t > 0 && t < 1 && u > 0) {
            float x = x1 + t * (x2 - x1);
            float y = y_1 + t * (y2 - y_1);
            return vec3(x, y, 0.0f);
        }
        else {
            return vec3(0.0f, 0.0f, 0.0f);
        }
    }
}