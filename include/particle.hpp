#pragma once
#include "vect2d.hpp"
#include "config.hpp"
#include "input.hpp"

typedef struct{
	vect2d pos, pos_old;
	vect2d vel, force;
	// int radius;
	double rho, rho_near;
	double press, press_near, sigma, beta;
} particle;

inline void world_to_sim(vect2d* pos){
	*pos = *pos / SCREEN_WIDTH;
	*pos = *pos * (SIM_W*2);
	*pos -= vect2d(SIM_W,SIM_W*2);
	pos->y *= -1;
}
inline void sim_to_world(vect2d* pos){
	pos->y *= -1;
	*pos += vect2d(SIM_W,SIM_W*2);
	*pos = *pos / (SIM_W*2);
	*pos = *pos * SCREEN_WIDTH;
}

void particle_init(particle* obj, vect2d pos, unsigned int radius);

void particle_update(particle* list, int size, double dt, input_t* input);

void particle_draw(particle* list, int size);