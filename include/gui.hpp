#pragma once
#include "input.hpp"
#include <vector>

class slider;

class ui{
    private:
        std::vector<slider*> sliders;
    public:
        ui();
        ~ui();
        void tick(input_t* input);
        void addSlider(const char* name, double* value, double min, double max);
        void printData();
};

class slider{
    private:
        int x;
        int y;
        int width;
        int height;
        double min;
        double max;
    public:
        double* value;
        char name[3];
        slider(const char* name, double* value, double min, double max);
        ~slider();
        void tick(input_t* input);
};