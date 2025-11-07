#include <stdio.h>
#include "graphics.hpp"
#include "input.hpp"
#include "timing.hpp"
#include "config.hpp"
#include "particle.hpp"
#include "gui.hpp"

input_t input;
particle* particles;
#define PARTICLES_X 80
#define PARTICLES_Y 40
#define TOTAL_PARTICLES PARTICLES_X*PARTICLES_Y

// GUI variables
ui* gui;
// double mouse_force = 0;
// double PRESSURE_FORCE = 7000000;
// double GRAVITY = 650;
// double TARGET_DENSITY = 0.0155;
// double SMOOTH_RADIUS = 65;

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
        }
    }
}

int main(int argc, char** argv) {    
    // Inits
    graphics_init(SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init();

    make_particles(vect2d(50,100), PARTICLES_X, PARTICLES_Y, 4, 8);

    // UI
    gui = new ui();
    // gui->addSlider("Fm", &mouse_force, 0, 1000);
    // gui->addSlider("Fp", &PRESSURE_FORCE, 0, 10000000);
    // gui->addSlider("G", &GRAVITY, 0, 1000);
    // gui->addSlider("TD", &TARGET_DENSITY, 0, 0.03);
    // gui->addSlider("SR", &SMOOTH_RADIUS, 0, 500);

    // Loop
    while (!input.quit) {
        // Input
        input_check(&input);

        // Timing
        double dt_real = timing_getDelta();
        double dt = 5/(double)1000;
        if(dt_real < dt){ // No overspeedy frames
            dt = dt_real;
        }
        
        // Erase
        graphics_clearScreen(_RGB(BKG_COLOR));
        
        // --------------- Critical Section
        // Update
        #ifdef __NVCC__
            cuda_particle_updateDensities(particles, TOTAL_PARTICLES);
            cuda_particle_update(particles, TOTAL_PARTICLES, dt, &input);
        #else
            particle_updateDensities(particles, TOTAL_PARTICLES);
            #if defined(_OPENMP)
                #pragma omp parallel for schedule(dynamic, 8)
            #endif
            for(int i = 0; i < TOTAL_PARTICLES; i++){
                particle_update(particles, TOTAL_PARTICLES, i, dt, &input);
            }
        #endif
        // --------------- END Critical Section

        // Draw
        for(int i = 0; i < TOTAL_PARTICLES; i++){
            particle_draw(&particles[i]);
        }

        //GUI
        gui->tick(&input);
        static char fps[12];
        sprintf(fps, "FPS - %2.2f", 1/dt_real);
        graphics_drawString(vect2d(0,10), fps, _RGB(255,255,255));

        // Swap buffers to display the frame
        graphics_swapBuffers();

        // Benchmark check
        #ifdef BENCHMARK
        frames++;
        if(frames >= SKIPPED_FRAMES){
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
    #ifndef BENCHMARK
        gui->printData();
    #endif

    graphics_deinit();
    input_deinit();
    return 0;
}