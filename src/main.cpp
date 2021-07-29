#include "main.hpp"

#include "RNG.hpp"
#include "SDL_utils.hpp"
#include "components.hpp"
#include "entity.hpp"
#include "event.hpp"
#include "keyboard.hpp"
#include "math.hpp"
#include "systems.hpp"
#include <SDL2/SDL.h>

SDL_Point ThrustImage1[] = {{-7, -3}, {-8, 0}, {-7, 3}};
SDL_Point ThrustImage2[] = {{-8, -5}, {-10, 0}, {-8, 5}};
SDL_Point ThrustImage3[] = {{-8, -5}, {-12, 0}, {-8, 5}};
SDL_Point ThrustImage4[] = {{-8, -5}, {-11, 0}, {-8, 5}};
SDL_Point ThrustImage5[] = {{-8, -5}, {-10, 0}, {-8, 5}};

const comPoints ThrustFrame1 = {&ThrustImage1[0], 3};
const comPoints ThrustFrame2 = {&ThrustImage2[0], 3};
const comPoints ThrustFrame3 = {&ThrustImage3[0], 3};
const comPoints ThrustFrame4 = {&ThrustImage4[0], 3};
const comPoints ThrustFrame5 = {&ThrustImage5[0], 3};

const comPoints *ThrustAnimation[] = {&ThrustFrame1, &ThrustFrame2, &ThrustFrame3, &ThrustFrame4, &ThrustFrame5};

SDL_Renderer *ren;
SDL_Texture *texPreTarget;
GameState state;

char getValidChar() {
	char result = 0;
	
	if (lastPressedKeycode == ' ' ||
	    (lastPressedKeycode >= '0' && lastPressedKeycode <= '9')) {
		result = lastPressedKeycode;
	}
	else if (lastPressedKeycode >= 'a' && lastPressedKeycode <= 'z') {
		result = lastPressedKeycode & 0b11011111; //our font data only supports uppercase letters
	}
	
	lastPressedKeycode = 0;
	return result;
}

void manageState(double deltaTime) {
	state.timer += deltaTime;

	switch (state.state) {
	case IN_LEVEL: {
		if (!state.hasAsters) {
			state.state = INBETWEEN_LEVEL;
			state.timer = 0;
		}

		if (state.state == IN_LEVEL) {
			state.UFOTimer += deltaTime;
			if (state.UFOTimer > state.nextUFO) {
				if (getRandBool()) makeUFO(UFO_BIG);
				else
					makeUFO(UFO_SMALL);
				state.nextUFO = getRandDouble(10, 20);
				state.UFOTimer = 0;
			}
		}
	} break;

	case INBETWEEN_LEVEL: {
		if (state.timer >= 4.0) {
			state.speedmod += 5; // TODO: make sure this is a good number
			state.level++;
			state.state = IN_LEVEL;
			for (int i = 0; i < state.level * 2 + 4; i++) {
				v2 pos;
				switch (getRandUint(1, 4)) {
				case 1: { // top
					pos.y = 0;
					pos.x = getRandUint(0, REN_WIDTH);
				} break;
				case 2: { // bottom
					pos.y = REN_HEIGHT;
					pos.x = getRandUint(0, REN_WIDTH);
				} break;
				case 3: { // left
					pos.y = getRandUint(0, REN_HEIGHT);
					pos.x = 0;
				} break;
				case 4: { // right
					pos.y = getRandUint(0, REN_HEIGHT);
					pos.x = REN_WIDTH;
				} break;
				}
				makeAsteroid(pos, getRandInt(30, 36));
			}
			state.timer = 0;
			state.UFOTimer = 0;
			state.nextUFO = getRandDouble(20, 30);
		}
	} break;

	case RESPAWN_COOLDOWN: {
		if (state.timer >= 2.0) {
			state.state = WAITING_FOR_RESPAWN;
			state.timer = 0;
			state.respawnMsgVisable = false;
		}
	} break;

	case WAITING_FOR_RESPAWN: {
		if (state.timer > 1.40) {
			state.timer = 0;
			state.respawnMsgVisable = !state.respawnMsgVisable;
			if (state.respawnMsgVisable) {
				state.respawnMsg = makeString("PRESS THE SPACE KEY TO RESPAWN", true, v2(REN_WIDTH / 2 - 8 * (30 / 2), REN_HEIGHT / 2));
				state.timer = 0;
			} else {
				delEntID(state.respawnMsg);
			}
		}

		if (keyIsPushed(SDL_SCANCODE_SPACE)) {
			state.playerEntID = makePlayer();
			if (state.respawnMsgVisable) {
				delEntID(state.respawnMsg);
				state.respawnMsgVisable = false;
				state.respawnMsg = 0;
			}
			state.state = IN_LEVEL;
			state.timer = 0;
			state.lives--;
		}
	} break;

	case GAME_OVER: {
		v2 pos = v2(REN_WIDTH/2 - 8*(9/2), REN_HEIGHT/2);
		renderString(pos, "GAME OVER");
		if (state.timer > 2.25) {
			state.state = CHECK_HIGH_SCORE;
			state.timer = 0.0f;
		}
	} break;
		/*
		  CHECK_HIGH_SCORE and ENTRY_HIGH_SCORE share a block together so they can use eachother's static variables.
		*/
	case CHECK_HIGH_SCORE: {
		static int cursorPos;
		cursorPos = 0;
            
		static int newEntryIndex;
		newEntryIndex = 0;
            
		static char newEntryPreviewBuffer[20];
            
		static int newEntryPreviewLen;
		newEntryPreviewLen = 0;
			
		lastPressedKeycode = 0; //stops us from capturing keycodes have been pressed in game play
			
		//compare cur score to highscores
		{
			bool newRecord = false;
			for (newEntryIndex = 0; newEntryIndex < NUM_OF_HIGH_SCORES; newEntryIndex++) {
				if (state.highScore[newEntryIndex].score < state.score) {
					newRecord = true;
					break;
				}
			}
				
			if (newRecord) {
				//move from this slot, to the bottom down by one slot
				for (int j = NUM_OF_HIGH_SCORES - 2; j >= newEntryIndex; j--) {
					//TODO: replace this with memmove()?
					memcpy(&state.highScore[j+1], &state.highScore[j], sizeof ScoreEntry);
				}
				//put new score value in table
				state.highScore[newEntryIndex].score = state.score;
				//set placeholder name
				memcpy(state.highScore[newEntryIndex].name, "___", 4);
					
				//populate entry preview
				newEntryPreviewLen = snprintf(newEntryPreviewBuffer, 20, "%3s%12d",
				                              state.highScore[newEntryIndex].name,
				                              state.highScore[newEntryIndex].score);
					
				//trans to name entry state
				state.state = ENTRY_HIGH_SCORE;
			}
				
			else {
				state.state = MAIN_MENU;
			}
		}
		break;
			
		case ENTRY_HIGH_SCORE:
            
			v2 pos = v2(REN_WIDTH/2 - 8*(14/2), (REN_HEIGHT * 12)/32);
			renderString(pos, "NEW HIGH SCORE");
			pos = v2(REN_WIDTH/2 - 8*(18/2), (REN_HEIGHT * 13)/32);
			renderString(pos, "ENTER YOUR INITALS");
			//print score entry while inputting
            
            
			pos = v2(REN_WIDTH/2 - 8*(newEntryPreviewLen/2), (REN_HEIGHT * 15)/32);
			renderString(pos, newEntryPreviewBuffer);
            
            
			char input = getValidChar();
			if (input != 0) {
				state.highScore[newEntryIndex].name[cursorPos] = input;
				newEntryPreviewLen = snprintf(newEntryPreviewBuffer, 20, "%3s%12d",
				                              state.highScore[newEntryIndex].name,
				                              state.highScore[newEntryIndex].score);
				cursorPos++;
				if (cursorPos == 3) {
					//we're done inputting names!
					//save to file, and then move on to the next game!
					FILE* file = fopen("highscore.data", "wb");
					fwrite(state.highScore, sizeof ScoreEntry, 10, file);
					state.state = MAIN_MENU;
					fclose(file);
				}
			}
	} break;
		
	case MAIN_MENU: {
		v2 pos = v2(REN_WIDTH/2 - 8*(9/2), (REN_HEIGHT * 7)/8);
		renderString(pos, "ASTEROIDS");
            
		pos = v2(REN_WIDTH/2 - 8*(24/2), (REN_HEIGHT * 2)/8);
		renderString(pos, "PRESS SPACE TO PLAY GAME");
            
		//TODO: we don't have to regenerate this every single frame...
		pos = v2(REN_WIDTH/2 - 8*(13/2), (REN_HEIGHT * 6)/16);
		renderString(pos, "TOP 10 SCORES");
		char buffer[20];
		for (int i = 0; i < NUM_OF_HIGH_SCORES; i++) {
			if (state.highScore[i].score == 0) {
				break;
			}
			int len = snprintf(buffer, 20, "%3s%12d", state.highScore[i].name,
			                   state.highScore[i].score);
                
			pos = v2(REN_WIDTH/2 - 8*(len/2), (REN_HEIGHT * (14+i))/32);
			renderString(pos, buffer);
		}
			
		if (keyIsPushed(SDL_SCANCODE_SPACE)) {
			state.state = INBETWEEN_LEVEL;
			state.playerEntID = makePlayer();
			state.timer = 0;
			state.level = -1;
			state.score = 0;
			state.lives = 3;
                
			for (std::vector<int>::iterator iter = entList.begin(); iter != entList.end();) {
				if (getAllComs(*iter).ast) {
					iter = delEntID(*iter);
				}
				else {
					iter++;
				}
			}
		}
	} break;
	case PAUSED: {
		v2 pos = v2(REN_WIDTH/2 - 8*(6/2), (REN_HEIGHT * 1)/8);
		renderString(pos, "PAUSED");
	} break;
	}

	state.hasAsters = false;
}

// I think levels should work like this:
// All Asters start with baseline speedmod
// All Asters spawned after that will have a random number pulled as an addition to this starting speedmod
// All the limits for this random number get higher as more Asters are destroied, eventually leading to a max amount.

/*
    Notes from real game:
    Asters always spawn from the edge of the screen.
        (I think the game assumes some overscan on the display? I might want to emulate that.)
    There is respawn protections, ship always spawns in the center, checks if there are asters nearby so it doesn't
   spawn the player unfairly. This measure doesn't prevent all deaths... We should let the player tell the game when to
   respawn them

    levels:
        1: 4 asters
        2: 6 asters
        3: 8 asters
        All have same speed, does the game just continue like this?

    There are 2 UFOs, one shoots randomly, the other aims at the player with random spread
        (we could make the random UFO target asters
*/
void test() {
#ifdef DEBUG

#endif
}

int main(int argc, char *argv[]) {
	test();
	initRNG();
	SDL_Init(SDL_INIT_VIDEO);

	
	state = {};
	state.state = MAIN_MENU;
	state.playerEntID = -1;

	state.windims.w = REN_WIDTH;
	state.windims.h = REN_HEIGHT;
	state.isFullscreen = false;

	{ // Load highscores from file
		FILE *hsFile = fopen("highscore.data", "rb");
		if (hsFile) {
			fread(state.highScore, sizeof(ScoreEntry), NUM_OF_HIGH_SCORES, hsFile);
			fclose(hsFile);
		}
		else { // there's no highscore file! let's generate some believable scores
			for (int index = NUM_OF_HIGH_SCORES; index >= 0; index--) {
				int difference = index - NUM_OF_HIGH_SCORES;
				state.highScore[index].score = difference * 140 + getRandInt(0, 140/5) * 5;
				state.highScore[index].name[0] = '_';
				state.highScore[index].name[1] = '_';
				state.highScore[index].name[2] = '_';
				state.highScore[index].name[3] = 0;
			}
		}
	}

	SDL_Window *win = SDL_CreateWindow("Asteroids", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, state.windims.w, state.windims.h, SDL_WINDOW_SHOWN);
	ren = SDL_CreateRenderer(win, 0, SDL_RENDERER_ACCELERATED);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	{ // checks renderer info for required features
		// NOTE: renInfo must be allocated memory
		SDL_RendererInfo *renInfo = (SDL_RendererInfo *)calloc(1, sizeof(SDL_RendererInfo));
		int error = SDL_GetRendererInfo(ren, renInfo);
		if (error) {
			printf("SDL error\n");
			return -1;
		}
		if ((renInfo->flags & SDL_RENDERER_TARGETTEXTURE) &&
		    (renInfo->max_texture_width >= REN_WIDTH && renInfo->max_texture_width >= REN_HEIGHT)) {
			free(renInfo);
		} else {
			printf("error, renderer does not support required features!\n");
			return -1;
		}
	}

	texPreTarget = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, REN_WIDTH, REN_HEIGHT);

	bool rendermode = true;
	bool quit = false;
	bool pause = false;

	int lastTime = 0;
	int curTime = SDL_GetTicks();
	double deltaTime = 0;
	SDL_Event event;
	while (!quit) {
		lastTime = curTime;
		curTime = SDL_GetTicks();
		deltaTime += ((double)(curTime - lastTime)) / 1000;
		if (deltaTime >= GAME_TICK_RATE) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYUP:
				case SDL_KEYDOWN: keyboardHandler(event); break;

				case SDL_WINDOWEVENT: {
					if (event.window.event == SDL_WINDOWEVENT_CLOSE) { quit = true; }
				} break;
				default: break;
				}
			}

			if (keyIsPressed(SDL_SCANCODE_Q)) { // quit
				quit = true;
			}

			if (state.playerEntID != -1) {
				if (keyIsPressed(SDL_SCANCODE_D)) { // rotate left
					comLocMap.at(state.playerEntID).rot -= (RAD(3) / 0.016) * deltaTime;
				}

				if (keyIsPressed(SDL_SCANCODE_F)) { // roatate right
					comLocMap.at(state.playerEntID).rot += (RAD(3) / 0.016) * deltaTime;
				}

				if (keyIsPushed(SDL_SCANCODE_J)) { // thrust
					comPoints *temp = (comPoints *)calloc(5, sizeof comPoints);
					// TODO: probably not the best thing to do when we push a key
					temp[0].pts = (SDL_Point *)calloc(3, sizeof SDL_Point);
					memcpy(temp[0].pts, &ThrustImage1, 3 * sizeof SDL_Point);
					temp[0].len = 3;

					temp[1].pts = (SDL_Point *)calloc(3, sizeof SDL_Point);
					memcpy(temp[1].pts, &ThrustImage2, 3 * sizeof SDL_Point);
					temp[1].len = 3;

					temp[2].pts = (SDL_Point *)calloc(3, sizeof SDL_Point);
					memcpy(temp[2].pts, &ThrustImage3, 3 * sizeof SDL_Point);
					temp[2].len = 3;

					temp[3].pts = (SDL_Point *)calloc(3, sizeof SDL_Point);
					memcpy(temp[3].pts, &ThrustImage4, 3 * sizeof SDL_Point);
					temp[3].len = 3;

					temp[4].pts = (SDL_Point *)calloc(3, sizeof SDL_Point);
					memcpy(temp[4].pts, &ThrustImage5, 3 * sizeof SDL_Point);
					temp[4].len = 3;

					addAnimationCom(state.playerEntID, temp, 5, GAME_TICK_RATE);
				}
				if (keyIsPressed(SDL_SCANCODE_J)) {
					double rot = comLocMap.at(state.playerEntID).rot;
					const v2 thrust(200, 0);
					mat2 rotMat ROT_MAT2(rot);
					comMovMap.at(state.playerEntID).acl = rotMat * thrust;
				}
				if (keyIsReleased(SDL_SCANCODE_J)) {
					delAnimationCom(state.playerEntID);
					comMovMap.at(state.playerEntID).acl = v2(0, 0);
				}

				if (keyIsPushed(SDL_SCANCODE_K)) { // fire
					mat2 rotMat ROT_MAT2(comLocMap.at(state.playerEntID).rot);
					v2 pos = comLocMap.at(state.playerEntID).pos + (rotMat * v2(8, 0));
					v2 vel = rotMat * v2(400, 0);
					makeBullet(pos, vel, comColMap.at(state.playerEntID).grp, state.playerEntID);
				}

				if (keyIsPushed(SDL_SCANCODE_P)) { // pause
					static int stateToRestore = -1;
					if (!pause) {
						pause = true;
						stateToRestore = state.state;
						state.state = PAUSED;
					}
					else {
						pause = false;
						state.state = stateToRestore;
						stateToRestore = -1;
					}
				}

				if (keyIsPushed(SDL_SCANCODE_SPACE)) { // hyperspace
				}

#ifdef DEBUG
				if (keyIsPushed(SDL_SCANCODE_H)) {
					comMovMap.at(state.playerEntID).acl = v2(0.0, 0.0);
					comMovMap.at(state.playerEntID).vel = v2(0.0, 0.0);
					printf("Stopping spaceship\n");
				}

				if (keyIsPressed(SDL_SCANCODE_UP)) { // free move commands
					comLocMap.at(state.playerEntID).pos.y -= 0.5;
				}
				if (keyIsPressed(SDL_SCANCODE_RIGHT)) { comLocMap.at(state.playerEntID).pos.x += 0.5; }
				if (keyIsPressed(SDL_SCANCODE_DOWN)) { comLocMap.at(state.playerEntID).pos.y += 0.5; }
				if (keyIsPressed(SDL_SCANCODE_LEFT)) { comLocMap.at(state.playerEntID).pos.x -= 0.5; }
#endif
			}

			// render scale
			if (keyIsPushed(SDL_SCANCODE_F1)) { // 1x
				// update window size
				if (state.isFullscreen) {
					SDL_SetWindowFullscreen(win, 0);
					state.isFullscreen = false;
				}
				state.windims = {0, 0, REN_WIDTH, REN_HEIGHT};
				SDL_SetWindowSize(win, state.windims.w, state.windims.h);
			}
			if (keyIsPushed(SDL_SCANCODE_F2)) { // 2x
				// update window size
				if (state.isFullscreen) {
					SDL_SetWindowFullscreen(win, 0);
					state.isFullscreen = false;
				}
				state.windims = {0, 0, REN_WIDTH * 2, REN_HEIGHT * 2};
				SDL_SetWindowSize(win, state.windims.w, state.windims.h);
			}
			if (keyIsPushed(SDL_SCANCODE_F3)) { // full screen, ratio
				SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
				state.isFullscreen = true;
				SDL_DisplayMode mode;
				SDL_GetWindowDisplayMode(win, &mode);
				// resize view based on screen dimentions
				int xFactor = mode.w / REN_WIDTH;
				int yFactor = mode.h / REN_HEIGHT;
				int factor = findMin(xFactor, yFactor);
				if (xFactor > factor) xFactor = factor;
				if (yFactor > factor) yFactor = factor;
				int width = 0;
				int height = 0;
				if (factor != 0) {
					width = factor * REN_WIDTH;
					height = factor * REN_HEIGHT;
				} else {
					width = mode.w;
					height = mode.h;
				}
				// center view
				state.windims = {(mode.w / 2) - (width / 2), (mode.h / 2) - (height / 2), width, height};
			}
			if (keyIsPushed(SDL_SCANCODE_F4)) { // full screen, streched
				SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
				state.isFullscreen = true;
				SDL_DisplayMode mode;
				SDL_GetWindowDisplayMode(win, &mode);
				state.windims = {0, 0, mode.w, mode.h};
			}

#ifdef DEBUG
			if (keyIsPushed(SDL_SCANCODE_R)) {
				rendermode = rendermode ? false : true;
				printf("rendermode set to %s\n", rendermode ? "points" : "colliders");
			}

			static double radius = 5.0;
			static int amount = 10;
			if (keyIsPushed(SDL_SCANCODE_W)) {
				amount += 1;
				printf("amount: %d\n", amount);
			}
			if (keyIsPushed(SDL_SCANCODE_S)) {
				amount -= 1;
				printf("amount: %d\n", amount);
			}
			if (keyIsPushed(SDL_SCANCODE_A)) {
				radius += 1.0;
				printf("radius: %f\n", radius);
			}
			if (keyIsPushed(SDL_SCANCODE_Z)) {
				radius -= 1.0;
				printf("radius: %f\n", radius);
			}
			if (keyIsPushed(SDL_SCANCODE_X)) { makeUFO(UFO_SMALL); }
			if (keyIsPushed(SDL_SCANCODE_C)) { makeUFO(UFO_BIG); }
			if (keyIsPushed(SDL_SCANCODE_E)) {
				comLocation loc = {v2(REN_WIDTH / 2, REN_HEIGHT / 2), 0};
				comPoints poi = {0, shipLen};
				poi.pts = (SDL_Point *)calloc(shipLen, sizeof SDL_Point);
				memcpy(poi.pts, shipPoints, shipLen * sizeof SDL_Point);
				makeDebris(&loc, &poi, radius, amount);
				free(poi.pts);
			}
			if (keyIsPressed(SDL_SCANCODE_L)) {
				for (auto iter = entList.begin(); iter != entList.end();) {
					if (comAstMap.count(*iter)) {
						iter = delEntID(*iter);
					} else {
						iter++;
					}
				}
			}
#endif

			SDL_SetRenderTarget(ren, texPreTarget);
			SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xff);
			SDL_RenderClear(ren);

			SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xff, 0xff);

			if (pause) deltaTime = 0;

			// draw score
			{
				v2 pos = v2(2, 2); // scorePos
				char buffer[100];
				sprintf(buffer, "SCORE %d", state.score);
				renderString(pos, buffer);
#ifdef DEBUG
				// draw frame time
				sprintf(buffer, "%f", deltaTime);
				pos.y += 25;
				renderString(pos, buffer);
#endif
			}

			// draw lives
			{
				v2 basePos(REN_WIDTH - (state.lives * 10) + 4, 8);
				for (int i = 0; i < state.lives; i++) {
					renderPoints(shipPoints, shipLen, basePos, RAD(-90));
					basePos.x += 10;
				}
			}

			// Handle entity stuff
			auto iter = entList.begin();
			while (iter != entList.end()) {
				bool die = false;

				AllComs coms = getAllComs(*iter);

				if (coms.ast) { state.hasAsters = true; }

				generateColliders(coms.col, coms.mov, deltaTime); // must happen before moveEntity does
				if (rendermode) renderPoints(coms.poi, coms.loc);
				else
					renderColliders(coms.loc, coms.col);
				renderRectangle(coms.loc, coms.rec);
				renderString(coms.loc, coms.txt);
				advanceAnimation(coms.ani, coms.loc, deltaTime);
				moveEntity(coms.mov, coms.loc, deltaTime);
				checkCollide(coms.col, coms.loc, *iter);
				die = doUFOai(*iter, coms.ufo, coms.mov, coms.loc, deltaTime, getAllComs(state.playerEntID).loc) || die;
				die = advanceLifetime(coms.lif, deltaTime) || die;
				if (die) {
					iter = delEntID(*iter); // goes to next element
				} else {
					iter++;
				}
			}

			// handle collision events
			if (!colEveList.empty()) {
				for (auto iter = colEveList.begin(); iter != colEveList.end(); iter = colEveList.erase(iter)) {
					processCollisionEvent(*iter);
				}
			}

			// handle spawn bullet events
			if (!bulSpawnEveList.empty()) {
				for (auto iter = bulSpawnEveList.begin(); iter != bulSpawnEveList.end(); iter = bulSpawnEveList.erase(iter)) {
					processBulletSpawnEvent(*iter);
				}
			}

			// state.playerEntID only, apply drag.
			if (state.playerEntID != -1) {
				comMovement mov = comMovMap.at(state.playerEntID);
				if (mov.vel.getMag() > 25) { // might want to make this smaller?
					mov.vel += mov.vel.getUnit() * -25 * deltaTime;
					comMovMap.at(state.playerEntID) = mov;
				}
			}

			manageState(deltaTime);

			// scale preTexTarget to the window size
			SDL_SetRenderTarget(ren, 0);
			SDL_SetRenderDrawColor(ren, 0x33, 0x33, 0x33, 0xff);
			SDL_RenderClear(ren);
			SDL_Rect src = {0, 0, REN_WIDTH, REN_HEIGHT};
			SDL_RenderCopy(ren, texPreTarget, &src, &state.windims);

			SDL_RenderPresent(ren);
			keyboardNextFrame();
			deltaTime = 0.f;
		}
	}
	if (state.playerEntID != -1) delEntID(state.playerEntID);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
