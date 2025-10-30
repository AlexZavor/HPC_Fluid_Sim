#include <stdio.h>
#include "graphics.hpp"
#include "input.hpp"
#include "timing.hpp"
#include "config.hpp"
#include "particle.hpp"

input_t input;
particle* particles;

void make_particles(vect2d pos, int width, int height, int particle_size, int particle_space){
    particles = (particle*)malloc(width*height*sizeof(particle));
    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            particle_init( &particles[y*(width) + x], pos+vect2d(x*particle_space, y*particle_space), particle_size);
        }
    }
}

int main(int argc, char** argv) {    
    // Inits
    graphics_init(SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init();

    particle ball;
    particle_init(&ball, vect2d(SCREEN_WIDTH/2, SCREEN_HEIGHT/2), 10);

    
    // Loop
    while (!input.quit) {
        input_check(&input);
        double dt = timing_getDelta();
        // printf("dt - %fms\n", dt*1000);
        
        // Erase
        graphics_clearScreen(_RGB(BKG_COLOR));
        
        // Update
        particle_update(&ball, dt, &input);

        // Draw
        particle_draw(&ball);

        // Swap buffers to display the frame
        graphics_swapBuffers();
        // Sleep a millisec to keep our frames from lagging my poor computer
        timing_sleepSeconds(0.001);
    }

    graphics_deinit();
    input_deinit();
    return 0;
}