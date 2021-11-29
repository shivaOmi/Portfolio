#include<iostream>
#include<limits>
#include"ray.h"
#include"hittablelist.h"
#include"sphere.h"
#include"camera.h"
#include"lambertian.h"
#include"metal.h"
#include"dielectric.h"

vec3 color(const ray& r, hittable *world, int depth){
    hit_record rec;
    if(world->hit(r,0.001,100000000.0f, rec)){
        ray scattered;
        vec3 attenuation;
        if(depth < 100 && rec.mat_ptr->scatter(r,rec,attenuation,scattered)){
            return attenuation*color(scattered, world,depth+1);
        }
        else{
            return vec3(0,0,0);
        }
    }
    else{
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5f * (unit_direction.y() + 1.0f);
        return ((1.0f - t)*vec3(1.0f,1.0f,1.0f) + t*vec3(0.5f,0.7f,1.0f));
    }
}

hittable *random_scene(){
    int n=500;
    hittable **list = new hittable*[n+1];
    list[0] = new sphere(vec3(0,-1000,0),1000, new lambertian(vec3(0.5,0.5,0.5)));
    int i = 1;

    for(int a = -11;a<11;a++){
        for(int b = -11;b<11;b++){
            float choose_mat = random_double();
            vec3 center(a+0.9*random_double(),0.2,b+0.9*random_double());
            if((center-vec3(4,0.2,0)).length() > 0.9){
                if(choose_mat < 0.8){
                    list[i++] = new sphere(center,0.2,new lambertian(vec3(random_double()*random_double(),random_double()*random_double(),random_double()*random_double())));
                }
                else if(choose_mat < 0.95){
                    list[i++] = new sphere(center, 0.2,new metal(vec3(0.5*(1 + random_double()),0.5*(1 + random_double()),0.5*(1 + random_double())),0.5*(1 + random_double())));
                }
                else{
                    list[i++] = new sphere(center, 0.2,new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0,1,0),1.0,new dielectric(1.5));
    list[i++] = new sphere(vec3(-4,1,0),1.0,new lambertian(vec3(0.4,0.2,0.1)));
    list[i++] = new sphere(vec3(4,1,0),1.0,new metal(vec3(0.7,0.6,0.5),0.0));

    return new hittable_list(list,i);
}

int main(){
    FILE *fptr = nullptr;
    int nx = 1920;
    int ny = 1080;
    const int ns = 100;

    if(fopen_s(&fptr,"magic.ppm","w") != 0){
        std::cout<<"Error in fopen().\n";
    }

    fprintf_s(fptr,"P3\n%d %d\n255\n",nx,ny);

    //vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
    //vec3 horizontal(4.0f,0.0f,0.0f);
    //vec3 vertical(0.0f, 2.0f, 0.0f);
    //vec3 origin(0.0f, 0.0f, 0.0f);
    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.0;

    /*hittable *list[5];
    list[0]= new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1,0.2,0.5)));
    list[1]= new sphere(vec3(0,-100.5,-1), 100.0, new lambertian(vec3(0.8,0.8,0.0)));
    list[2]= new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8,0.6,0.2),0.3));
    list[3]= new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    list[4]= new sphere(vec3(-1,0,-1), -0.45, new dielectric(1.5));
    hittable *world = new hittable_list(list,5);*/
    hittable *world = random_scene();
    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus);

    for(int j=ny-1; j>= 0; j--){
        for(int i=0; i<nx; i++){
            vec3 col(0,0,0);
            for(int s=0;s<ns;s++){
                float u = float(i + random_double()) / float(nx);
                float v = float(j + random_double()) / float(ny);
                ray r = cam.get_ray(u,v);
                col += color(r,world,0);
            }
            col /= float(ns);
            
            int ir = int(255.99*sqrt(col[0]));
            int ig = int(255.99*sqrt(col[1]));
            int ib = int(255.99*sqrt(col[2]));

            fprintf_s(fptr,"%d %d %d\n", ir,ig,ib);
        }
    }
}