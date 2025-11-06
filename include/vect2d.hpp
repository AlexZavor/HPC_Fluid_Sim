#pragma once

#include <math.h>
#include <random>

#define ZERO_VECT (vect2d(0,0))
#define NORMAL_VECT (vect2d(1,0))

class vect2d {
public:
    double x;
    double y;
public:
    double getAngle();
    void normalize();
    double getMag();
    vect2d rotate(double angle);

    vect2d operator+(vect2d const& obj);
    void operator+=(vect2d const& obj);
    vect2d operator*(double const& obj);
    void operator*=(double const& obj);
    vect2d operator/(double const& obj);
    vect2d operator%(vect2d const& obj);
    vect2d operator-(vect2d const& obj);
    vect2d operator-=(vect2d const& obj);

    vect2d(double x, double y);
    vect2d(){}
};
