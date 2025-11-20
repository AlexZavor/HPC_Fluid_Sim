#include <stdio.h>
#include "graphics.hpp"
#include "input.hpp"
#include "timing.hpp"
#include "config.hpp"
#include "particle.hpp"
#include "gui.hpp"

input_t input;
particle* particles;
#define PARTICLES_X 40
#define PARTICLES_Y 90
#define TOTAL_PARTICLES PARTICLES_X*PARTICLES_Y

// GUI variables
// ui* gui;

#ifdef BENCHMARK
double frametimes[BENCH_FRAMES];
long frames = 0;
#endif


void make_particles(vect2d pos, int width, int height, int particle_size, int particle_space){
    particles = (particle*)malloc(width*height*sizeof(particle));
    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            vect2d pos_noise = vect2d(rand()%100/(double)10000,rand()%100/(double)10000);
            particle_init( &particles[y*(width) + x], pos+vect2d(x*particle_space, y*particle_space) + pos_noise, particle_size);
            world_to_sim(&(particles[y*(width) + x].pos));
        }
    }
}

int main(int argc, char** argv) {    
    // Inits
    graphics_init(SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init();

    make_particles(vect2d(150,20), PARTICLES_X, PARTICLES_Y, 4, 7);

    // UI
    // gui = new ui();

    // Loop
    while (!input.quit) {
        // Input
        input_check(&input);

        // Timing
        double dt_real = timing_getDelta();
        
        // Erase
        graphics_clearScreen(_RGB(BKG_COLOR));
        
        // --------------- Critical Section
        // Update
        particle_update(particles, TOTAL_PARTICLES, &input);
        // --------------- END Critical Section

        // Draw
        particle_draw(particles, TOTAL_PARTICLES);

        //GUI
        // gui->tick(&input);
        static char fps[12];
        sprintf(fps, "FPS - %2.2f", 1/dt_real);
        graphics_drawString(vect2d(0,10), fps, _RGB(255,255,255));

        // Swap buffers to display the frame
        graphics_swapBuffers();

        // Benchmark check
        #ifdef BENCHMARK
        frames++;
        if(frames > SKIPPED_FRAMES){
            frametimes[frames-SKIPPED_FRAMES] = dt_real;
        }
        if(frames >= BENCH_FRAMES + SKIPPED_FRAMES){
            input.quit = true;
            double avg_ftime = 0;
            for(int i = 0; i < BENCH_FRAMES; i++){
                avg_ftime += frametimes[i];
            }
            avg_ftime /= BENCH_FRAMES;
            printf("Avg frame time: %fms\n", avg_ftime*1000);
        }
        #endif
    }

    free(particles);

    graphics_deinit();
    input_deinit();
    return 0;
}