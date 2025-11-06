#pragma once

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

#define BKG_COLOR 0,5,10
#define PART_COLOR 0,0,255

// Physics Defines
// #define GRAVITY 1000
// #define GRAVITY 0
extern double GRAVITY;
#define BOUNCE_CONST 0.5

// #define MOUSE_FORCE 10
extern double mouse_force;

// Pressure defines
#define SMOOTH_RADIUS 50
// #define TARGET_DENSITY 0.01
extern double TARGET_DENSITY;
// #define PRESSURE_FORCE 1500000
extern double PRESSURE_FORCE;