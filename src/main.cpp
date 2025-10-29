#include <stdio.h>
#include "graphics.hpp"
#include "input.hpp"
#include "timing.hpp"
#include "vect2d.hpp"
#include "config.hpp"

input_t input;

int main(int argc, char** argv) {    
    // Inits
    graphics_init(SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init();
    static float x,y;
    static float vx,vy;

    x = SCREEN_WIDTH/2; y = SCREEN_HEIGHT/2;
    vy = 0;

    
    // Loop
    while (!input.quit) {
        input_check(&input);
        double dt = timing_getDelta();
        // printf("dt - %fms\n", dt*1000);
        
        // Erase
        graphics_clearScreen(_RGB(BKG_COLOR));
        
        // Update
        vy += 1000*dt;// gravity
        if (y+20 + vy*dt > SCREEN_HEIGHT){//bounce
            vy = -vy*0.7;
        }
        y += vy*dt;// velocity

        // Draw
        graphics_fillCircle(vect2d(x,y), 20, _RGB(0,0,255));

        // Swap buffers to display the frame
        graphics_swapBuffers();
        // Sleep a millisec to keep our frames from lagging my poor computer
        timing_sleepSeconds(0.001);
    }

    graphics_deinit();
    input_deinit();
    return 0;
}