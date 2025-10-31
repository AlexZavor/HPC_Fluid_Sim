#pragma once
#include "vect2d.hpp"
#include "input.hpp"

typedef struct{
	vect2d pos;
	vect2d vel;
	int radius;
	double density;
} particle;

void particle_init(particle* obj, vect2d pos, unsigned int radius);

void particle_update(particle* list, int size, int point_index, double dt, input_t* input);

void particle_updateDensities(particle* list, int size);

void particle_draw(particle* obj);