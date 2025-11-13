#pragma once

#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 700

#define SIM_W (50)								// The size of the world

#define BKG_COLOR 0,5,10
#define PART_COLOR 0,0,255


#define BOUNCE_CONST 0.7


// Benchmark defines
#define BENCH_FRAMES 600
#define SKIPPED_FRAMES 3

// CUDA defines instead of externs
// #ifdef __NVCC__
	#define mouse_force 3000
	#define PRESSURE_FORCE 30000000
	#define GRAVITY 2000
	#define TARGET_DENSITY 0.0155
	#define SMOOTH_RADIUS 70
// #else
// 	// Physics Defines
// 	extern double GRAVITY;
// 	extern double mouse_force;

// 	// Pressure defines
// 	extern double SMOOTH_RADIUS;
// 	extern double TARGET_DENSITY;
// 	extern double PRESSURE_FORCE;
// #endif