#pragma once

#include "math.hpp"
#include <SDL2/SDL.h>
#include <unordered_map>

/////COM LOCATION START
struct comLocation {
	v2 pos;
	double rot;
};

extern std::unordered_map<int, comLocation> comLocMap;

void addLocationCom(int, v2, double);
void delLocationCom(int);
/////COM LOCATION END

/////COM POINTS START
struct comPoints {
	SDL_Point *pts;
	size_t len;
};

extern std::unordered_map<int, comPoints> comPoiMap;

void addPointsCom(int, SDL_Point *, int);
void delPointsCom(int);
/////COM POINTS END

/////COM MOVMENT START
struct comMovement {
	v2 vel; // spd is rotated by comLoc's
	v2 acl;
	double angVel; // does not rotate speed vectors, just rotates the loc
	double angAcl;
};

extern std::unordered_map<int, comMovement> comMovMap;

void addMovementCom(int, v2, v2, double, double);
void addMovementCom(int, v2, v2); // TODO: DEPERCATED, remove
void delMovementCom(int);
/////COM MOVEMENT END

/////COM RECTANGLE START
struct comRectangle {
	v2 dim;
};

extern std::unordered_map<int, comRectangle> comRecMap;

void addRectangleCom(int, v2);
void delRectangleCom(int);
/////COM RECTANGLE END

/////COM COLLIDER START
struct v2Pair {
	v2 one, two;
};

struct comCollider {
	v2Pair *ptPairs;
	int len;
	int grp;
	bool depsOnMov;
};

extern std::unordered_map<int, comCollider> comColMap;

void addColliderCom(int, v2Pair *, int, int, bool);
void delColliderCom(int);
/////COM COLLIDER END

/////COM ASTEROID START
struct comAsteroid {
	int size;
};

extern std::unordered_map<int, comAsteroid> comAstMap;

void addAsteroidCom(int, int);
void delAsteroidCom(int);
/////COM ASTEROID END

/////COM LIFETIME START
struct comLifetime {
	double length;
	double elapsed;
};

extern std::unordered_map<int, comLifetime> comLifMap;

void addLifetimeCom(int, double);
void delLifetimeCom(int);
/////COM LIFETIME END

/////COM SHOOTER START
struct comShooter {
	int maxBullets;
	int curBullets;
};

extern std::unordered_map<int, comShooter> comShrMap;

void addShooterCom(int, int);
void delShooterCom(int);
/////COM SHOOTER END

/////COM SHOT START
struct comShot {
	int owner; // entID owner
};

extern std::unordered_map<int, comShot> comShtMap;

void addShotCom(int, int);
void delShotCom(int);
/////COM SHOT END

/////COM TEXT START
struct comText {
	char *str; /// null term'd
	bool isConst;
};

extern std::unordered_map<int, comText> comTxtMap;

void addTextCom(int, char *, bool);
void delTextCom(int);
/////COM TEXT END

/////COM UFO START
struct comUFO {
	int size;
	double timer;
	double nextTurn;
	int turnState;
	v2 lastPos;
	int direction;
};

#define UFO_H_SPEED (100)
#define UFO_V_SPEED (0)

#define UFO_BIG (1)
#define UFO_SMALL (0)

#define UFO_LEFT (-1)
#define UFO_RIGHT (1)

#define UFO_STRAIGHT (1)
#define UFO_DIAGONAL (0)

extern std::unordered_map<int, comUFO> comUFOMap;

void addUFOCom(int, int, v2, int);
void delUFOCom(int);
/////COM UFO END

/////COM ANIMATION START
struct comAnimation {
	comPoints *images;
	int totalImages;
	int curImage;
	double rate; // how long it takes to advance to the next image in seconds.
	double elapsedTime;
};

extern std::unordered_map<int, comAnimation> comAniMap;

void addAnimationCom(int, comPoints *, int, double);
void delAnimationCom(int);
/////COM ANIMATION END