#pragma once

#ifdef __host__
    #include <cuda.h>
    #define CU_FLAG __host__ __device__
#else
    #define CU_FLAG
#endif

#include <math.h>
#include <random>

#define ZERO_VECT (vect2d(0,0))
#define NORMAL_VECT (vect2d(1,0))

class vect2d {
public:
    double x;
    double y;
public:
    CU_FLAG double getAngle(){
        return atan(y/x);
    }
    CU_FLAG void normalize(){
        double mag = this->getMag();
        x /= mag;
        y /= mag;
    }
    CU_FLAG double getMag(){
        return sqrt((x*x) + (y*y));
    }
    CU_FLAG double getMag2(){
        return (x*x) + (y*y);
    }
    CU_FLAG vect2d rotate(double angle){
        double px = x * cos(angle) - y * sin(angle);
        double py = x * sin(angle) + y * cos(angle);
        x = px;
        y = py;
        return *this;
    }

    CU_FLAG vect2d operator+(vect2d const& obj){
        vect2d value;
        value.x = obj.x + x;
        value.y = obj.y + y;
        return value;
    }
    CU_FLAG void operator+=(vect2d const& obj){
        this->x += obj.x;
        this->y += obj.y;
        return;
    }
    CU_FLAG vect2d operator*(double const& obj){
        vect2d value;
        value.x = obj * x;
        value.y = obj * y;
        return value;
    }

    CU_FLAG double operator*(const vect2d& b) const { return x*b.x + y*b.y; }
    CU_FLAG void operator*=(double const& obj){
        this->x *= obj;
        this->y *= obj;
        return;
    }
    CU_FLAG vect2d operator/(double const& obj){
        vect2d value;
        value.x = x / obj;
        value.y = y / obj;
        return value;
    }
    CU_FLAG vect2d operator%(vect2d const& obj){
        vect2d value = vect2d(x,y);
        if(x > obj.x){
            value.x = 0;
        }
        else if(x < 0){
            value.x = obj.x;
        }
        if(y > obj.y){
            value.y = 0;
        }
        else if(y < 0){
            value.y = obj.y;
        }
        return value;
    }
    CU_FLAG vect2d operator-(vect2d const& obj){
        vect2d value;
        value.x = x - obj.x;
        value.y = y - obj.y;
        return value;
    }
    CU_FLAG vect2d operator-=(vect2d const& obj){
        this->x -= obj.x;
        this->y -= obj.y;
        return *this;
    }

    CU_FLAG vect2d(double x, double y){
        this->x = x;
        this->y = y;
    }
    CU_FLAG vect2d(){}
};
