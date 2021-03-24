#pragma once
#include <SDL2/SDL.h>

void keyboardHandler(SDL_Event e);

void keyboardNextFrame();
bool keyIsHeld(int);
bool keyIsPressed(int);
bool keyIsReleased(int);
bool keyIsPushed(int);

extern char lastPressedKeycode;
