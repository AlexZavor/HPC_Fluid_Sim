#include "vect2d.hpp"

vect2d vect2d::operator+(vect2d const& obj){
    vect2d value;
    value.x = obj.x + x;
    value.y = obj.y + y;
    return value;
}

vect2d vect2d::operator*(float const &obj) {
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

float vect2d::getAngle(){
    return atan(y/x);
}

float vect2d::getMag(){
    return sqrt((x*x) + (y*y));
}

vect2d vect2d::rotate(float angle) {
    float px = x * cos(angle) - y * sin(angle);
    float py = x * sin(angle) + y * cos(angle);
    x = px;
    y = py;
    return *this;
}

vect2d::vect2d(float x, float y) {
    this->x = x;
    this->y = y;
}

void vect2d::normalize(){
    float mag = this->getMag();
    x /= mag;
    y /= mag;
}