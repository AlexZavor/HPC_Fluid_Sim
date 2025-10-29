#include "graphics.hpp"
#include <stdio.h>
#include "config.hpp"

static Display* display;
static Window window;
static GC gc;
static unsigned int window_w, window_h;
static Pixmap buffer;  // Off-screen buffer for double buffering

static inline void setColor(unsigned long color){
    static unsigned long fg_color = 0;
    if(color != fg_color){
        XSetForeground(display, gc, color);
        fg_color = color;
    }
}

Display* graphics_getDisplay() {
    return display;
}

Window graphics_getWindow() {
    return window;
}

int graphics_init(int width, int height)
{
    // Open the display connection
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        return -1;
    }

    int screen_num = DefaultScreen(display);

    // Create a simple window
    window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen_num), // Parent window (root window of the screen)
        0, 0, // x, y position
        width, height, // width, height
        1, // border_width
        BlackPixel(display, screen_num), // border_pixel
        WhitePixel(display, screen_num) // background_pixel
    );
    window_w = width; window_h = height;

    
    // get GC
    gc = DefaultGC(display, screen_num);
    
    // Create off-screen buffer
    buffer = XCreatePixmap(display, window, width, height, 
                          DefaultDepth(display, screen_num));
    
    // Map the window to the screen (make it visible)
    XMapWindow(display, window);

    return 0;
}

int graphics_deinit()
{
    if (display != NULL)
    {
        XFreePixmap(display, buffer);  // Free the off-screen buffer
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    }
    return 0;
}

void graphics_clearScreen(unsigned long color)
{
    setColor(color);
    // Clear the off-screen buffer
    XFillRectangle(display, buffer, gc, 0, 0, window_w, window_h);
}

void graphics_drawCircle(vect2d pos, unsigned int r, unsigned long color)
{
    setColor(color);
    // Draw to off-screen buffer
    XFillArc(display, buffer, gc, pos.x-r, pos.y-r, r*2, r*2, 0 * 64, 360 * 64);
}

void graphics_fillCircle(vect2d pos, unsigned int r, unsigned long color)
{
    setColor(color);
    // Draw to off-screen buffer
    XFillArc(display, buffer, gc, pos.x-r, pos.y-r, r*2, r*2, 0 * 64, 360 * 64);
}

// function to swap buffers at the end of frame
void graphics_swapBuffers(void)
{
    // Copy off-screen buffer to window
    XCopyArea(display, buffer, window, gc,
              0, 0, window_w, window_h,  // source x,y,w,h
              0, 0);                     // dest x,y
    XFlush(display);
}
