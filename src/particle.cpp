#include "particle.hpp"
#include "graphics.hpp"
#include "config.hpp"

void particle_init(particle* obj, vect2d pos, unsigned int radius){
	obj->pos = pos;
	obj->vel = ZERO_VECT;
	obj->radius = radius;
}

void particle_update(particle* obj, double dt, input_t* input){
	#define vel (obj->vel)
	#define pos (obj->pos)


	// Forces
	vel.y += GRAVITY*dt; // Gravity
	if(input->mouseLeft){ // Magic mouse
		vect2d mouse_vect = vect2d(input->mouseX, input->mouseY);
		vect2d force_vect = mouse_vect - pos;
		// printf("x-%d \t y-%d\n", input->mouseX, input->mouseY);
		vel = vel + (force_vect*10*dt);
	}

	pos = pos + vel*dt;// velocity

	// Colision
	if (pos.y > SCREEN_HEIGHT-obj->radius){ // floor
		pos.y = SCREEN_HEIGHT-obj->radius;
		vel.y = -vel.y*BOUNCE_CONST;
	}
	if (pos.y < obj->radius){ // ceiling
		pos.y = obj->radius;
		vel.y = -vel.y*BOUNCE_CONST;
	}
	if (pos.x < obj->radius){ // L wall
		pos.x = obj->radius;
		vel.x = -vel.x*BOUNCE_CONST;
	}
	if (pos.x > SCREEN_WIDTH-obj->radius){ // R wall
		pos.x = SCREEN_WIDTH-obj->radius;
		vel.x = -vel.x*BOUNCE_CONST;
	}

	#undef vel
	#undef pos
}

void particle_draw(particle* obj){
	graphics_fillCircle(obj->pos, obj->radius, _RGB(0,0,255));
	graphics_drawCircle(obj->pos, obj->radius, _RGB(0,0,128));
}