#ifndef MY_VEC3
#define MY_VEC3

#include <cmath>
#include <iostream>

template <typename T>
class vec3{
    T vec[3];

    vec3(): vec{0, 0, 0} {}
    vec3(T v0, T v1, T v2): vec{v0, v1, v2} {}

    T x() const { return vec[0]; }
    T y() const { return vec[1]; }
    T z() const { return vec[2]; }

    // overrides 
    T& operator[](int i){
        return vec[i];
    }

    vec3 operator*(T scalar){
        return vec3(vec[0] * scalar, vec[1] * scalar, vec[2] * scalar);
    }

    vec3 operator/(T scalar){
        return vec3(vec[0] / scalar, vec[1] / scalar, vec[2] / scalar);
    }

    vec3 operator+(const vec3<T> other){
        return vec3(vec[0] + other[0],
                    vec[1] + other[1],
                    vec[2] + other[2]);
    }

    vec3 operator-(const vec3<T> other){
        return vec3(vec[0] - other[0],
                    vec[1] - other[1],
                    vec[2] - other[2]);
    }

};



#endif
