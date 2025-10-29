#pragma once

typedef struct {
    // Mouse state
    int mouseX;
    int mouseY;
    bool mouseLeft;
    bool mouseRight;
    bool mouseMiddle;
    
    // Keyboard state
    bool arrowUp;
    bool arrowDown;
    bool arrowLeft;
    bool arrowRight;
    
    // General state
    bool quit;
} input_t;

int input_init();

int input_deinit();

void input_check(input_t* input);