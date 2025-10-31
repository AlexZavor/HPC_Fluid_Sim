#pragma once
#include "vect2d.hpp"
#include "input.hpp"

typedef struct{
	vect2d pos;
	vect2d vel;
	int radius;
} particle;

void particle_init(particle* obj, vect2d pos, unsigned int radius);

void particle_update(particle* obj, double dt, input_t* input, particle* particle_list, int particle_list_len);

void particle_draw(particle* obj);