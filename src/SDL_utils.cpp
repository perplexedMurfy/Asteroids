#include "SDL_utils.hpp"

#include <windows.h>
#include <SDL2/SDL.h>

void SDL_DrawTexturelessRect (SDL_Renderer* rend, SDL_Rect rect, SDL_Color color) {
	SDL_Color c = {0};
	SDL_GetRenderDrawColor (rend, &c.r, &c.g, &c.b, &c.a);
    
	SDL_SetRenderDrawColor (rend, color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect (rend, &rect);
	SDL_RenderFillRect (rend, &rect);
	SDL_SetRenderDrawColor (rend, c.r, c.g, c.b, c.a);
}

void SDL_DrawTexturelessRect (SDL_Renderer* rend, SDL_Rect rect, Color color) {
	SDL_DrawTexturelessRect (rend, rect, color.toSDL_Color());
}

int findMin (int a, int b) {
    if (a < b) { return a; }
    else {return b; }
    return 0;
}

void showMsg (const char* msg) {
	if (strcmp (msg, "") != 0) {
		MessageBox (NULL, msg, NULL, MB_OK);
		SDL_ClearError();
	}
	//else, no error
}

