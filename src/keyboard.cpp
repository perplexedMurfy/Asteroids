#include <memory.h>
#include "keyboard.hpp"

static bool curKeyboard[512] = {};
static bool lastKeyboard[512] = {};

char lastPressedKeycode = 0;

void keyboardHandler (SDL_Event e) {
	if (e.key.keysym.scancode < 512 && e.key.keysym.scancode >= 0) {
		if (e.type == SDL_KEYUP) {
			curKeyboard[e.key.keysym.scancode] = false;
		}
		else if (e.type == SDL_KEYDOWN) {
            curKeyboard[e.key.keysym.scancode] = true;
            lastPressedKeycode = e.key.keysym.sym;
		}
	}
}

void keyboardNextFrame () {
	memcpy (lastKeyboard, curKeyboard, 512);
}

bool keyIsHeld (int scancode) {
	return curKeyboard[scancode] && lastKeyboard[scancode];
}

bool keyIsPressed (int scancode) {
	return curKeyboard[scancode];
}

bool keyIsReleased (int scancode) {
	return !curKeyboard[scancode] && lastKeyboard[scancode];
}

bool keyIsPushed (int scancode) {
	return curKeyboard[scancode] && !lastKeyboard[scancode];
}


