#pragma once
#include <SDL2/SDL.h>

struct Color {
	float r, g, b, a;
	SDL_Color toSDL_Color () {
		return {(uint8_t)(r * 0xff), (uint8_t)(g * 0xff),
            (uint8_t)(b * 0xff), (uint8_t)(a * 0xff)};
	}
};

void SDL_DrawTexturelessRect (SDL_Renderer*, SDL_Rect, SDL_Color);
void SDL_DrawTexturelessRect (SDL_Renderer*, SDL_Rect, Color);

#define SDL_ReportError() (showMsg(SDL_GetError()))
void showMsg(const char*);

int findMin(int, int);

#ifdef DEBUG
#define D_bingo (printf("bingo\n"))

#define D_pInt(x, y) {\
printf(x);\
printf("%d\n", y);\
}
#else //TODO: does this actually dummy them out?
#define D_bingo ()
#define D_pInt(x, y) ()
#endif

#define SizeOfArray(X) (sizeof X / sizeof *X)