#include <stdio.h>
#include "graphics.hpp"
#include "input.hpp"
#include "timing.hpp"
#include "config.hpp"
#include "particle.hpp"

input_t input;
particle* particles;
#define PARTICLES_X 40
#define PARTICLES_Y 40
#define TOTAL_PARTICLES PARTICLES_X*PARTICLES_Y

void make_particles(vect2d pos, int width, int height, int particle_size, int particle_space){
    particles = (particle*)malloc(width*height*sizeof(particle));
    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            vect2d pos_noise = vect2d(rand()%100/(double)10000,rand()%100/(double)10000);
            particle_init( &particles[y*(width) + x], pos+vect2d(x*particle_space, y*particle_space) + pos_noise, particle_size);
        }
    }
}

int main(int argc, char** argv) {    
    // Inits
    graphics_init(SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init();

    make_particles(vect2d(200,50), PARTICLES_X, PARTICLES_Y, 3, 7);


    // Loop
    while (!input.quit) {
        input_check(&input);
        double dt = timing_getDelta();
        // printf("dt - %fms\n", dt*1000);
        
        // Erase
        graphics_clearScreen(_RGB(BKG_COLOR));
        
        // Update
        for(int i = 0; i < TOTAL_PARTICLES; i++){
            particle_update(&particles[i], dt, &input, particles, TOTAL_PARTICLES);
        }

        // Draw
        for(int i = 0; i < TOTAL_PARTICLES; i++){
            particle_draw(&particles[i]);
        }

        // Swap buffers to display the frame
        graphics_swapBuffers();
    }

    free(particles);

    graphics_deinit();
    input_deinit();
    return 0;
}