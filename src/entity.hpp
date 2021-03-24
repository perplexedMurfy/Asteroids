#pragma once
#include "SDL_utils.hpp"
#include "components.hpp"
#include "event.hpp"
#include <vector>

extern std::vector<int> entList;

struct AllComs {
	comPoints *poi;
	comLocation *loc;
	comMovement *mov;
	comRectangle *rec;
	comCollider *col;
	comAsteroid *ast;
	comLifetime *lif;
	comShooter *shr;
	comShot *sht;
	comText *txt;
	comUFO *ufo;
	comAnimation *ani;
};

int getEntID();
AllComs getAllComs(int);
std::vector<int>::iterator delEntID(int);

// 16 x 10
const SDL_Point shipPoints[] = {{8, 0}, {-8, 5}, {-5, 0}, {-8, -5}, {8, 0}};
const size_t shipLen = SizeOfArray(shipPoints);

const SDL_Point UFOPointsSmall[] = {{-4, -4}, {4, -4}, {2, -8}, {-2, -8}, {-4, -4}, {-6, 0},
                                    {6, 0},   {4, -4}, {6, 0},  {2, 4},   {-2, 4},  {-6, 0}};
const v2Pair UFOColsSmall[] = {{{-4, -4}, {-2, -8}}, {{-2, -8}, {2, -8}}, {{2, -8}, {4, -4}}, {{4, -4}, {6, 0}},
                               {{6, 0}, {2, 4}},     {{2, 4}, {-2, 4}},   {{-2, 4}, {-6, 0}}, {{-6, 0}, {-4, -4}}};

const SDL_Point UFOPointsBig[] = {{-8, -8}, {8, -8}, {4, -12}, {-4, -12}, {-8, -8}, {-12, 0},
                                  {12, 0},  {8, -8}, {12, 0},  {4, 8},    {-4, 8},  {-12, 0}};
const v2Pair UFOColsBig[] = {{{-8, -8}, {-4, -12}}, {{-4, -12}, {4, -12}}, {{4, -12}, {8, -8}}, {{8, -8}, {12, 0}},
                             {{12, 0}, {4, 8}},     {{4, 8}, {-4, 8}},     {{-4, 8}, {-12, 0}}, {{-12, 0}, {-8, -8}}};

// Maybe limit -8 to -7?

int makePlayer();
int makeBullet(v2, v2, int, int);
int makeBullet(BulletSpawnEvent);
int makeAsteroid(v2, int);
int makeUFO(int size);
int makeString(char *, bool, v2);
void makeDebris(comLocation *, comPoints *, double, int);