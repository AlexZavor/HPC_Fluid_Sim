#include "particle.hpp"
#include "graphics.hpp"
#include "config.hpp"
#include <math.h>

void particle_init(particle* obj, vect2d pos, unsigned int radius){
	obj->pos = pos;
	obj->vel = ZERO_VECT;
	obj->radius = radius;
}

double smoothKernel(double radius, double dist){
	if(dist>=radius)return 0;
	static double volume = M_PI * pow(radius, 4)/6;
	return (radius-dist) * (radius-dist) / volume;
}

double smoothKernelDerivative(double radius, double dist){
	if(dist>=radius) return 0;
	// double f = (radius*radius) - (dist*dist);
	static double scale = (double)12 / ((double)M_PI * pow(radius, 4));
	// const double scale = 0.000001;
	return (dist-radius)*scale;
}

double calcDensity(particle* list, int size, int point_index){
	double density = 0;

	vect2d point = list[point_index].pos;
	for(int i = 0; i < size; i++){
		if(i == point_index){continue;}
		double dist = (list[i].pos - point).getMag();
		if (dist > SMOOTH_RADIUS) continue;
		double influence = smoothKernel(SMOOTH_RADIUS, dist);
		density += influence;
	}

	return density;
}

void particle_updateDensities(particle* list, int size){
	for(int i = 0; i < size; i++){
		particle* particle = &list[i];
		particle->density = calcDensity(list, size, i);
		if (particle->density == 0) 
			particle->density = 0.00000000000001;
	}
}

double densityToPressure(double density){
	double densityError = density-TARGET_DENSITY;
	double pressure = densityError * PRESSURE_FORCE;
	return pressure;
}

vect2d calcGradient(particle* list, int size, int point_index){
	vect2d gradient = ZERO_VECT;
	
	vect2d point = list[point_index].pos;
	for(int i = 0; i < size; i++){
		if(i == point_index){continue;}
		vect2d diff = (list[i].pos - point);
		double dist = diff.getMag();
		vect2d dir;
		if(dist == 0){
			dir = NORMAL_VECT;
		}else{
			dir = diff/dist;
		}
		double slope = smoothKernelDerivative(SMOOTH_RADIUS, dist);
		double density = list[point_index].density;
		gradient += dir*slope*-1*densityToPressure(density)/density;
		// printf("%f\t%f\t%f\t%f\t%f\n", slope, density, dir.x, dir.y, dist);
	}

	return gradient;
}

		// printf("%f\t%f\t%f\t%f\t%f\n", diff.x, diff.y, dir.x, dir.y, dist);
		// exit(-1);


void particle_update(particle* list, int size, int point_index, double dt, input_t* input){
	#define vel (list[point_index].vel)
	#define pos (list[point_index].pos)

	particle* obj = &list[point_index];

	// Forces
	vel.y += GRAVITY*dt; // Gravity
	if(input->mouseLeft){ // Magic mouse
		vect2d mouse_vect = vect2d(input->mouseX, input->mouseY);
		vect2d force_vect = mouse_vect - pos;
		vel += (force_vect*mouse_force*dt);
	}	
	vect2d pressure = calcGradient(list, size, point_index) / obj->density; // pressure
	// if(!std::isnan(pressure.x) && !std::isnan(pressure.y)){
		vel += (pressure*-dt);
	// }else{
	// 	pos = ZERO_VECT;
	// 	printf("%f\n", pressure.x);
	// }


	// Velocity Clamping
	// if(vel.getMag()>(100/dt)){
	// 	vel = vel/2;
	// }
	pos += vel*dt;// velocity

	// Collision
	if (pos.y > SCREEN_HEIGHT-obj->radius){ // floor
		pos.y = SCREEN_HEIGHT-obj->radius;
		vel.y = -vel.y*BOUNCE_CONST;
	}
	else if (pos.y < obj->radius){ // ceiling
		pos.y = obj->radius;
		vel.y = -vel.y*BOUNCE_CONST;
	}
	if (pos.x < obj->radius){ // L wall
		pos.x = obj->radius;
		vel.x = -vel.x*BOUNCE_CONST;
	}
	else if (pos.x > SCREEN_WIDTH-obj->radius){ // R wall
		pos.x = SCREEN_WIDTH-obj->radius;
		vel.x = -vel.x*BOUNCE_CONST;
	}

	#undef vel
	#undef pos

}

void particle_draw(particle* obj){
	double density = (obj->density-TARGET_DENSITY)*100000;

	if(density > 255)density = 255;
	if(density < -255)density = -255;
	
	graphics_fillCircle(obj->pos, obj->radius, _RGB(density>0?density:0, 0 ,density<0?-density:0));
	graphics_drawCircle(obj->pos, obj->radius, _RGB(PART_COLOR/2));
}