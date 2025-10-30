#include "vect2d.hpp"

vect2d vect2d::operator+(vect2d const& obj){
    vect2d value;
    value.x = obj.x + x;
    value.y = obj.y + y;
    return value;
}

vect2d vect2d::operator*(double const &obj) {
    vect2d value;
    value.x = obj * x;
    value.y = obj * y;
    return value;
}

vect2d vect2d::operator%(vect2d const &obj) {
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

vect2d vect2d::operator-(vect2d const &obj) {
    vect2d value;
    value.x = x - obj.x;
    value.y = y - obj.y;
    return value;
}

double vect2d::getAngle(){
    return atan(y/x);
}

double vect2d::getMag(){
    return sqrt((x*x) + (y*y));
}

vect2d vect2d::rotate(double angle) {
    double px = x * cos(angle) - y * sin(angle);
    double py = x * sin(angle) + y * cos(angle);
    x = px;
    y = py;
    return *this;
}

vect2d::vect2d(double x, double y) {
    this->x = x;
    this->y = y;
}

void vect2d::normalize(){
    double mag = this->getMag();
    x /= mag;
    y /= mag;
}