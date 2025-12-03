#ifndef LIGHT_H
#define LIGHT_H
#include<glm/glm.hpp>
using namespace glm;
struct Light {
    vec3 position = vec3(0.f, 5.f, 5.f);
    vec4 ambient = vec4(0.1f, 0.1f, 0.1f, 1.f);
    vec4 diffuse = vec4(0.8f, 0.8f, 0.8f, 1.f);
    vec4 specular = vec4(1.f, 1.f, 1.f, 1.f);
    Light(const vec3 &position, const vec4 &ambient, const vec4 &diffuse, const vec4 &specular)
        : position(position), ambient(ambient), diffuse(diffuse), specular(specular)
    {}
    Light() = default;
    void setPosition(const vec3 &position)
    {
        this->position = position;
    }
    void changeIntensity(bool i)
    {
        if (i) {
            ambient *= 1.1f;
            diffuse *= 1.1f;
            specular *= 1.1f;
        }
        else {
            ambient /= 1.1f;
            diffuse /= 1.1f;
            specular /= 1.1f;
        }
    }
};
struct Material {
    vec4 ambient = vec4(1.f, 1.f, 1.f, 1.f);
    vec4 diffuse = vec4(1.f, 1.f, 1.f, 1.f);
    vec4 specular = vec4(1.f, 1.f, 1.f, 1.f);
    float shininess = 32.f;
    //string imgName = "";
    Material(const vec4 &ambient, const vec4 &diffuse, const vec4 &specular, float shininess)
        : ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
    {}
    Material() = default;
};
#endif