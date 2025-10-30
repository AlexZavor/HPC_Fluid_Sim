#include "input.hpp"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>  // For XLookupString
#include "graphics.hpp"
#include <stdio.h>

static XEvent event;
static Display* display;
static Window window;

int input_init()
{
    display = graphics_getDisplay();
    window = graphics_getWindow();
    
    // Select input events we want to handle
    XSelectInput(display, window, 
        KeyPressMask | KeyReleaseMask |     // Keyboard events
        ButtonPressMask | ButtonReleaseMask | // Mouse button events
        PointerMotionMask |                  // Mouse movement
        ExposureMask);                       // Window exposure
    
    return 0;
}

int input_deinit()
{
    return 0;
}

void input_check(input_t* input)
{
    while (XPending(display)) {  // Handle all pending events
        XNextEvent(display, &event);
        switch (event.type) {
            case Expose:
                // Handle expose events
                // graphics_clearScreen(_RGB(0,0,12));
                break;

            case KeyPress:
            case KeyRelease:
                {
                    bool isPress = (event.type == KeyPress);
                    KeySym keysym = XLookupKeysym(&event.xkey, 0);
                    
                    // Update arrow key states
                    if (keysym == XK_Up)    input->arrowUp = isPress;
                    if (keysym == XK_Down)  input->arrowDown = isPress;
                    if (keysym == XK_Left)  input->arrowLeft = isPress;
                    if (keysym == XK_Right) input->arrowRight = isPress;
                    
                    // Exit on Escape
                    if (keysym == XK_Escape) input->quit = isPress;
                }
                break;

            case ButtonPress:
            case ButtonRelease:
                {
                    bool isPress = (event.type == ButtonPress);
                    switch (event.xbutton.button) {
                        case Button1: input->mouseLeft = isPress; break;
                        case Button2: input->mouseMiddle = isPress; break;
                        case Button3: input->mouseRight = isPress; break;
                    }
                }
                break;

            case MotionNotify:
                input->mouseX = event.xmotion.x;
                input->mouseY = event.xmotion.y;
                break;
        }
    }
}
