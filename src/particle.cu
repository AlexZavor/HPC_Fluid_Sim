#include "particle.hpp"
#include "graphics.hpp"
#include <math.h>

/*
*
*  SPH Code from a neat example Created by:
*	Brandon Pelfrey
*	blog.brandonpelfrey.com (no longer active, check the wayback machine)
*	https://web.archive.org/web/20090722233436/blog.brandonpelfrey.com/?p=303
*
*
*	Cuda code written by Alex Carter.
*
*/

#define SCHEDULE schedule(dynamic, 1)

#define G (.02f * .25)							// Gravitational Constant for our simulation

#define spacing (2.f)							// Spacing of particles
#define k (spacing / 1000.0)			        // Far pressure weight
#define k_near (k*10)						    // Near pressure weight
#define rest_density (4)						// Rest Density
#define r (spacing*1.25)						// Radius of Support
#define rsq (r*r)								// ... squared for performance stuff

#define bottom (0)								// The floor of the world

// Cuda Kernels
#ifdef __NVCC__
#include <cuda.h>

#define BLOCK_SIZE 512
static particle* list_d;
static input_t* input_d;
static int first = true;


__global__ void cuda_update(particle* particles, int size, input_t* input){
	// Cuda: Find location
	int i = blockDim.x*blockIdx.x + threadIdx.x;
	if(i < size){
	
	// UPDATE
	//
	// This modified verlet integrator has dt = 1 and calculates the velocity
	// For later use in the simulation. 

	// For each particles i ...
	// Normal verlet stuff
	particles[i].pos_old = particles[i].pos;
	particles[i].pos += particles[i].vel;

	// Apply the currently accumulated forces
	particles[i].pos += particles[i].force;

	// Restart the forces with gravity only. We'll add the rest later.
	particles[i].force = vect2d(0,-G);

	// Calculate the velocity for later.
	particles[i].vel = particles[i].pos - particles[i].pos_old;

	// If the velocity is really high, we're going to cheat and cap it.
	// This will not damp all motion. It's not physically-based at all. Just
	// a little bit of a hack.
	double max_vel = 2.f;
	double vel_mag = particles[i].vel.getMag2();
	// If the velocity is greater than the max velocity, then cut it in half.
	if(vel_mag > max_vel*max_vel)
		particles[i].vel = particles[i].vel * .5f;

	// If the particle is outside the bounds of the world, then
	// Make a little spring force to push it back in.
	if(particles[i].pos.x < -SIM_W) particles[i].force.x -= (particles[i].pos.x - -SIM_W) / 8;
	if(particles[i].pos.x >  SIM_W) particles[i].force.x -= (particles[i].pos.x - SIM_W) / 8;
	if(particles[i].pos.y < bottom) particles[i].force.y -= (particles[i].pos.y - bottom) / 8;
	if(particles[i].pos.y > SIM_W*2)particles[i].force.y -= (particles[i].pos.y - SIM_W*2) / 8;

	// Handle the mouse attractor. 
	// It's a simple spring based attraction to where the mouse is.			
	vect2d mouse_vect = vect2d(input->mouseX, input->mouseY);
	world_to_sim(&mouse_vect);
	// world_to_sim(&mouse_vect);
	double attr_dist2 = (particles[i].pos - mouse_vect).getMag2();
	const double attr_l = SIM_W/4;
	if(input->mouseLeft||input->mouseRight){ // Magic mouse
		if( attr_dist2 < attr_l*attr_l )
			particles[i].force -= (particles[i].pos - mouse_vect) / 512 * (input->mouseLeft?1:-1);// * (input->mouseRight?-1:1);
	}

	// Reset the necessary items.
	particles[i].rho = particles[i].rho_near = 0;
}}

__global__ void cuda_density(particle* particles, int size, input_t* input){
	// Cuda: Find location
	int i = blockDim.x*blockIdx.x + threadIdx.x;
	if(i < size){
    
	// DENSITY 
	//
	// Calculate the density by basically making a weighted sum
	// of the distances of neighboring particles within the radius of support (r)

	// For each particle ...
	particles[i].rho = particles[i].rho_near = 0;

	// We will sum up the 'near' and 'far' densities.
	double d=0, dn=0;

	// Now look at every other particle
	for(int j=0; j < size; ++j)
	{
		// We only want to look at each pair of particles just once. 
		// And do not calculate an interaction for a particle with itself!
		if(j == i) continue;

		// The vector separating the two particles
		vect2d rij = particles[j].pos - particles[i].pos;

		// Along with the squared distance between
		double rij_len2 = rij.getMag2();

		// If they're within the radius of support ...
		if(rij_len2 < rsq)
		{
			// Get the actual distance from the squared distance.
			double rij_len = sqrt(rij_len2);

			// And calculated the weighted distance values
			double q = 1 - rij_len / r;
			double q2 = q*q;
			double q3 = q2*q;

			d += q2;
			dn += q3;
		}
	}

	particles[i].rho        += d;
	particles[i].rho_near   += dn;

}}

__global__ void cuda_pressure(particle* particles, int size, input_t* input){
	// Cuda: Find location
	int i = blockDim.x*blockIdx.x + threadIdx.x;
	if(i < size){

	// PRESSURE
	//
	// Make the simple pressure calculation from the equation of state.
	particles[i].press = k * (particles[i].rho - rest_density);
	particles[i].press_near = k_near * particles[i].rho_near;

}}

__global__ void cuda_pressuref(particle* particles, int size, input_t* input){
	// Cuda: Find location
	int i = blockDim.x*blockIdx.x + threadIdx.x;
	if(i < size){

	// PRESSURE FORCE
	//
	// We will force particles in or out from their neighbors
	// based on their difference from the rest density.

	// For each particle ...
	vect2d dX = ZERO_VECT;

	// Now look at every other particle
	for(int j=0; j < size; ++j)
	{
		// We only want to look at each pair of particles just once. 
		// And do not calculate an interaction for a particle with itself!
		if(j == i) continue;

		// The vector separating the two particles
		vect2d rij = particles[j].pos - particles[i].pos;

		// Along with the squared distance between
		double rij_len2 = rij.getMag2();            

		// If they're within the radius of support ...
		if(rij_len2 < rsq)
		{
			// Get the actual distance from the squared distance.
			double rij_len = sqrt(rij_len2);

			// And calculated the weighted distance values
			double q = 1 - rij_len / r;
			double q2 = q*q;

			// calculate the force from the pressures calculated above
			double dm = (particles[i].press + particles[j].press) * q +
				(particles[i].press_near + particles[j].press_near) * q2;

			// Get the direction of the force
			rij.normalize();
			vect2d D = rij * dm;
			dX += D;
		}
	}

	particles[i].force -= dX;

}}

__global__ void cuda_viscosity(particle* particles, int size, input_t* input){
	// Cuda: Find location
	int i = blockDim.x*blockIdx.x + threadIdx.x;
	if(i < size){

	// VISCOSITY
	//
	// This simulation actually may look okay if you don't compute 
	// the viscosity section. The effects of numerical damping and 
	// surface tension will give a smooth appearance on their own.
	// Try it.

	// For each particle
	// Now look at every other particle
	for(int j=0; j < size; ++j)
	{
		// We only want to look at each pair of particles just once. 
		// And do not calculate an interaction for a particle with itself!
		if(j == i) continue;

		// The vector separating the two particles
		vect2d rij = particles[j].pos - particles[i].pos;

		// Along with the squared distance between
		double rij_len2 = rij.getMag2();            

		// If they're within the radius of support ...
		if(rij_len2 < rsq)
		{
			// Get the actual distance from the squared distance.
			double rij_len = sqrt(rij_len2);

			// And calculated the weighted distance values
			double q = 1 - rij_len / r;


			vect2d rij = particles[j].pos - particles[i].pos;
			double l = (rij).getMag();
			// double q = l / r;

			vect2d rijn = (rij / l);
			// Get the projection of the velocities onto the vector between them.
			double u = (particles[i].vel - particles[j].vel) * rijn;
			if(u > 0)
			{
				// Calculate the viscosity impulse between the two particles
				// based on the quadratic function of projected length.
				vect2d I = rijn * (1 - q) * (particles[j].sigma * u + particles[j].beta * u*u);

				// Apply the impulses on the two particles
				particles[i].vel -= I * 0.5;
			}
		}

	}	
}}


#endif

void particle_init(particle* obj, vect2d pos, unsigned int radius){
	obj->pos = pos;
	obj->pos_old = pos + (vect2d(rand()%1000/(1000.0), rand()%1000/(1000.0))*0.001);
	obj->vel = ZERO_VECT;
	obj->force = ZERO_VECT;
	obj->sigma = .1f;
	obj->beta = 0.f;
}


void particle_update(particle* particles, int size, input_t* input){

	#ifdef __NVCC__
	// allocate mem, only on first
	if(first){
		first = false;
		int bytes = size * sizeof(particle);
  		cudaMalloc((void**) &list_d, bytes);
  		cudaMemcpy(list_d, particles, bytes, cudaMemcpyHostToDevice);
  		cudaMalloc((void**) &input_d, sizeof(input_t));
	}
	// update input each frame
	cudaMemcpy(input_d, input, sizeof(input_t), cudaMemcpyHostToDevice);
	
	// block size
	dim3 blocks(ceil(size/(float)BLOCK_SIZE), 1, 1);
  	dim3 threads(BLOCK_SIZE, 1, 1);

	// launch kernel
	cuda_update<<<blocks, threads>>>(list_d, size, input_d);
	cuda_density<<<blocks, threads>>>(list_d, size, input_d);
	cuda_pressure<<<blocks, threads>>>(list_d, size, input_d);
	cuda_pressuref<<<blocks, threads>>>(list_d, size, input_d);
	cuda_viscosity<<<blocks, threads>>>(list_d, size, input_d);
  	cudaDeviceSynchronize();
  	cudaMemcpy(particles, list_d, size*sizeof(particle), cudaMemcpyDeviceToHost);  
	#else


	// UPDATE
	//
	// This modified verlet integrator has CU_FLAG = 1 and calculates the velocity
	// For later use in the simulation. 

	// For each particles i ...

	#if defined(_OPENMP)
	#pragma omp parallel for SCHEDULE
	#endif
	for(int i=0; i < size; ++i)
	{
		// Normal verlet stuff
		particles[i].pos_old = particles[i].pos;
		particles[i].pos += particles[i].vel;

		// Apply the currently accumulated forces
		particles[i].pos += particles[i].force;

		// Restart the forces with gravity only. We'll add the rest later.
		particles[i].force = vect2d(0,-G);

		// Calculate the velocity for later.
		particles[i].vel = particles[i].pos - particles[i].pos_old;

		// If the velocity is really high, we're going to cheat and cap it.
		// This will not damp all motion. It's not physically-based at all. Just
		// a little bit of a hack.
		double max_vel = 2.f;
		double vel_mag = particles[i].vel.getMag2();
		// If the velocity is greater than the max velocity, then cut it in half.
		if(vel_mag > max_vel*max_vel)
			particles[i].vel = particles[i].vel * .5f;

		// If the particle is outside the bounds of the world, then
		// Make a little spring force to push it back in.
		if(particles[i].pos.x < -SIM_W) particles[i].force.x -= (particles[i].pos.x - -SIM_W) / 8;
		if(particles[i].pos.x >  SIM_W) particles[i].force.x -= (particles[i].pos.x - SIM_W) / 8;
		if(particles[i].pos.y < bottom) particles[i].force.y -= (particles[i].pos.y - bottom) / 8;
		if(particles[i].pos.y > SIM_W*2)particles[i].force.y -= (particles[i].pos.y - SIM_W*2) / 8;

		// Handle the mouse attractor. 
		// It's a simple spring based attraction to where the mouse is.			
		vect2d mouse_vect = vect2d(input->mouseX, input->mouseY);
		world_to_sim(&mouse_vect);
		// world_to_sim(&mouse_vect);
		double attr_dist2 = (particles[i].pos - mouse_vect).getMag2();
		const double attr_l = SIM_W/4;
		if(input->mouseLeft||input->mouseRight){ // Magic mouse
			if( attr_dist2 < attr_l*attr_l )
				particles[i].force -= (particles[i].pos - mouse_vect) / 512 * (input->mouseLeft?1:-1);// * (input->mouseRight?-1:1);
		}
	
		// Reset the necessary items.
		particles[i].rho = particles[i].rho_near = 0;
	}
    
	// DENSITY 
	//
	// Calculate the density by basically making a weighted sum
	// of the distances of neighboring particles within the radius of support (r)

	// For each particle ...
	#if defined(_OPENMP)
	#pragma omp parallel for SCHEDULE
	#endif
	for(int i=0; i < size; ++i)
	{
		particles[i].rho = particles[i].rho_near = 0;

		// We will sum up the 'near' and 'far' densities.
		double d=0, dn=0;

		// Now look at every other particle
		for(int j=0; j < size; ++j)
		{
			// We only want to look at each pair of particles just once. 
			// And do not calculate an interaction for a particle with itself!
		if(j == i) continue;

			// The vector separating the two particles
			vect2d rij = particles[j].pos - particles[i].pos;

			// Along with the squared distance between
			double rij_len2 = rij.getMag2();            

			// If they're within the radius of support ...
			if(rij_len2 < rsq)
			{
				// Get the actual distance from the squared distance.
				double rij_len = sqrt(rij_len2);

				// And calculated the weighted distance values
				double q = 1 - rij_len / r;
				double q2 = q*q;
				double q3 = q2*q;

				d += q2;
				dn += q3;
			}
		}

		particles[i].rho        += d;
		particles[i].rho_near   += dn;
	}

	// PRESSURE
	//
	// Make the simple pressure calculation from the equation of state.
	#if defined(_OPENMP)
	#pragma omp parallel for SCHEDULE
	#endif
	for(int i=0; i < size; ++i)
	{
		particles[i].press = k * (particles[i].rho - rest_density);
		particles[i].press_near = k_near * particles[i].rho_near;
	}

	// PRESSURE FORCE
	//
	// We will force particles in or out from their neighbors
	// based on their difference from the rest density.

	// For each particle ...
	#if defined(_OPENMP)
	#pragma omp parallel for SCHEDULE
	#endif
	for(int i=0; i < size; ++i)
	{
		vect2d dX = ZERO_VECT;

		// Now look at every other particle
		for(int j=0; j < size; ++j)
		{
			// We only want to look at each pair of particles just once. 
			// And do not calculate an interaction for a particle with itself!
			if(j == i) continue;

			// The vector separating the two particles
			vect2d rij = particles[j].pos - particles[i].pos;

			// Along with the squared distance between
			double rij_len2 = rij.getMag2();            

			// If they're within the radius of support ...
			if(rij_len2 < rsq)
			{
				// Get the actual distance from the squared distance.
				double rij_len = sqrt(rij_len2);

				// And calculated the weighted distance values
				double q = 1 - rij_len / r;
				double q2 = q*q;   

                // The vector from particle i to particle j
                // Vec2 rij = particles[j].pos - particles[i].pos;

                // calculate the force from the pressures calculated above
                double dm = (particles[i].press + particles[j].press) * q +
                    (particles[i].press_near + particles[j].press_near) * q2;

                // Get the direction of the force
				rij.normalize();
                vect2d D = rij * dm;
                dX += D;
            }
		}

		particles[i].force -= dX;
	}

	// VISCOSITY
	//
	// This simulation actually may look okay if you don't compute 
	// the viscosity section. The effects of numerical damping and 
	// surface tension will give a smooth appearance on their own.
	// Try it.

	// For each particle
	#if defined(_OPENMP)
	#pragma omp parallel for SCHEDULE
	#endif
	for(int i=0; i < size; ++i)
	{
        // Now look at every other particle
		for(int j=0; j < size; ++j)
		{
			// We only want to look at each pair of particles just once. 
			// And do not calculate an interaction for a particle with itself!
			if(j == i) continue;

			// The vector separating the two particles
			vect2d rij = particles[j].pos - particles[i].pos;

			// Along with the squared distance between
			double rij_len2 = rij.getMag2();            

			// If they're within the radius of support ...
			if(rij_len2 < rsq)
			{
				// Get the actual distance from the squared distance.
				double rij_len = sqrt(rij_len2);

				// And calculated the weighted distance values
				double q = 1 - rij_len / r;


                vect2d rij = particles[j].pos - particles[i].pos;
                double l = (rij).getMag();

                vect2d rijn = (rij / l);
                // Get the projection of the velocities onto the vector between them.
                double u = (particles[i].vel - particles[j].vel) * rijn;
                if(u > 0)
                {
                    // Calculate the viscosity impulse between the two particles
                    // based on the quadratic function of projected length.
                    vect2d I = rijn * (1 - q) * (particles[j].sigma * u + particles[j].beta * u*u);

                    // Apply the impulses on the two particles
                    particles[i].vel -= I * 0.5;
                }
            }

		}
	}

	#endif
}

void particle_draw(particle* list, int size){
	 vect2d pos[size];
	 unsigned long col[size];

	for(int i = 0; i < size; i++){
		double density = (list[i].rho + list[i].rho_near - rest_density)*100;

		if(density > UINT8_MAX)density = UINT8_MAX;
		if(density < -UINT8_MAX)density = -UINT8_MAX;

		col[i] = _RGB(density>0?density:0, 0 ,density<0?-density:0);

		pos[i] = list[i].pos;
		sim_to_world(&pos[i]);
		
	}
	
	for(int i = 0; i < size; i++)
		graphics_fillCircle(pos[i], 4, col[i]);

	for(int i = 0; i < size; i++)
		graphics_drawCircle(pos[i], 4, _RGB(PART_COLOR/2));
}