#include "gui.hpp"
#include "config.hpp"
#include "graphics.hpp"


static int count = 0;
#define WIDTH 15
#define HEIGHT 100

#define SLIDER_KNOB_RADIUS 5
#define SLIDER_CHANNEL_WIDTH 2

ui::ui() {
}

ui::~ui(){
    for(auto s: sliders){
        delete s;
    }
    sliders.clear();
}

void ui::tick(input_t *input) {
    for(auto s: sliders){
        s->tick(input);
    }
    if(input->space){
        printData();
        input->space = false; // cheeky OSD
    }
}

void ui::addSlider(const char *name, double *value, double min, double max)
{
    sliders.push_back(new slider(name, value, min, max));
}

void ui::printData()
{
    for(auto s: sliders){
        printf("%s-%f\t",s->name, *s->value);
    }
    printf("\n");
}

slider::slider(const char* name, double* value, double min, double max){
    count++;
    x = SCREEN_WIDTH-(count*WIDTH);
    y = SLIDER_KNOB_RADIUS;
    width = WIDTH;
    height = HEIGHT;
    this->value = value;
    this->max = max;
    this->min = min;
    for(int i = 0; i < 2; i++){
        this->name[i] = name[i];
    }
    this->name[2] = '\0';
}

slider::~slider()
{
}

void slider::tick(input_t* input){
    if(input->mouseLeft){
        if(input->mouseX > x && input->mouseX < x+width){
        if(input->mouseY > y && input->mouseY < y+height){
            *value = (( ((input->mouseY-y)/((float)height)) * (max-min) )+min);
        }}
    }
    graphics_fillRect(vect2d(x+((width-SLIDER_CHANNEL_WIDTH)/2), y), SLIDER_CHANNEL_WIDTH, height, _RGB(128,128,128));
    graphics_fillCircle(vect2d(x+(width/2), ((((*value)-min)/(max-min))*height)+y ), SLIDER_KNOB_RADIUS, _RGB(255,255,255));
    graphics_drawString(vect2d(x,height+y + 20), name, _RGB(150,150,255));
}

