#pragma once

#include <math.h>
#include <random>

#define ZERO_VECT (vect2d(0,0))
#define NORMAL_VECT (vect2d(1,0))

class vect2d {
public:
    float x;
    float y;
public:
    float getAngle();
    vect2d operator+(vect2d const& obj);
    vect2d operator*(float const& obj);
    vect2d operator%(vect2d const& obj);
    vect2d operator-(vect2d const& obj);
    void normalize();
    float getMag();
    vect2d rotate(float angle);

    vect2d(float x, float y);
    vect2d(){}
};