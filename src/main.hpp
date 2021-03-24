#pragma once

#include <SDL2/SDL.h>

// also controls how big game world is
#define REN_WIDTH (640)
#define REN_HEIGHT (512)

// controls size of window. REN_WIDTH/HEIGHT will be scaled up to these dimentions.
// DEPERCATED
#define WIN_WIDTH (1280)
#define WIN_HEIGHT (1024)

#define GAME_TICK_RATE (0.016)

#define NUM_OF_HIGH_SCORES (10)

extern SDL_Renderer *ren;
extern SDL_Texture *texPreTarget;

enum {
	IN_LEVEL,            /// Player is currently in a level
	INBETWEEN_LEVEL,     /// Player has just destroied all Asteroids
	RESPAWN_COOLDOWN,    /// Player has just died, waiting to show respawn prompt
	WAITING_FOR_RESPAWN, /// Player can respawn themselves
	GAME_OVER,           /// Player has died and ran out of lives
	MAIN_MENU,           /// what it says on the tin. we can start playing the game from here.
	ENTRY_HIGH_SCORE,    /// Player is entering their name on the highscore board.
	CHECK_HIGH_SCORE,    /// Checking if we made a new record.
	PAUSED,              /// Game is paused.
};

struct ScoreEntry {
	int score;
	char name[4];
};

struct GameState {
	int state;       /// Current state
	int playerEntID; /// the ID of the player

	// seperate this timer into spefic ones
	double timer; /// generic timer used for state-dependant things.

	// game information
	double speedmod;
	int level;
	int score;
	int lives;

	// manges UFO spawning when IN_LEVEL state.
	double nextUFO;
	double UFOTimer;

	// manages the respawn message when in WAITING_FOR_RESPAWN
	bool respawnMsgVisable;
	int respawnMsg;

	// used to detect when a player has cleared a level
	bool hasAsters;

	// manages window size.
	SDL_Rect windims;
	bool isFullscreen;

	ScoreEntry highScore[NUM_OF_HIGH_SCORES];
};

extern GameState state;
