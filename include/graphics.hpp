// X11 wrapper for this project for drawing to the screen using a frame buffer.

#pragma once
#include <X11/Xlib.h>
#include "vect2d.hpp"

// ======= System Functions ==========
// Get X11 display and window handles
Display* graphics_getDisplay();
Window graphics_getWindow();

// simple function for getting colors
inline unsigned long _RGB(int r,int g, int b)
{
    return b + (g<<8) + (r<<16);
}

// Initialize graphics window and open it
// returns 0 when successful, negative otherwise
int graphics_init(int width, int height);

// clean up graphics, close window. ext.
// returns 0 when successful, negative otherwise
int graphics_deinit();


// ======= Drawing Functions ========

void graphics_clearScreen(unsigned long color);

void graphics_drawCircle(vect2d pos, unsigned int r, unsigned long color);
void graphics_fillCircle(vect2d pos, unsigned int r, unsigned long color);

void graphics_fillRect(vect2d pos, unsigned int w, unsigned int h, unsigned long color);

void graphics_drawString(vect2d pos, const char* text, unsigned long color);

// Swap the back buffer to the screen and flush the display.
// Call this once at the end of each frame after all drawing is complete.
void graphics_swapBuffers(void);
