#include "entity.hpp"

#include "RNG.hpp"
#include "SDL_utils.hpp"
#include "main.hpp"
#include <stdexcept>

// keeps track of currently active entIDs
std::vector<int> entList;

// give new entities their entID
int getEntID() {
	static int id = 0;
	id++;
	entList.push_back(id);
	return id;
}

/**
 * Convience function to get all the components that
 * an entity has
 */
AllComs getAllComs(int entID) {
	AllComs result;
	try { result.poi = &(comPoiMap.at(entID)); } catch (std::out_of_range) { result.poi = 0; }
	try { result.loc = &(comLocMap.at(entID)); } catch (std::out_of_range) { result.loc = 0; }
	try { result.mov = &(comMovMap.at(entID)); } catch (std::out_of_range) { result.mov = 0; }
	try { result.rec = &(comRecMap.at(entID)); } catch (std::out_of_range) { result.rec = 0; }
	try { result.col = &(comColMap.at(entID)); } catch (std::out_of_range) { result.col = 0; }
	try { result.ast = &(comAstMap.at(entID)); } catch (std::out_of_range) { result.ast = 0; }
	try { result.lif = &(comLifMap.at(entID)); } catch (std::out_of_range) { result.lif = 0; }
	try { result.shr = &(comShrMap.at(entID)); } catch (std::out_of_range) { result.shr = 0; }
	try { result.sht = &(comShtMap.at(entID)); } catch (std::out_of_range) { result.sht = 0; }
	try { result.txt = &(comTxtMap.at(entID)); } catch (std::out_of_range) { result.txt = 0; }
	try { result.ufo = &(comUFOMap.at(entID)); } catch (std::out_of_range) { result.ufo = 0; }
	try { result.ani = &(comAniMap.at(entID)); } catch (std::out_of_range) { result.ani = 0; }
	return result;
}

// apperently this is how you use the iterator type. good to keep that in mind for the future
std::vector<int>::iterator delEntID(int entID) {
	delPointsCom(entID);
	delLocationCom(entID);
	delMovementCom(entID);
	delRectangleCom(entID);
	delColliderCom(entID);
	delAsteroidCom(entID);
	delLifetimeCom(entID);
	delShooterCom(entID);
	delShotCom(entID);
	delTextCom(entID);
	delUFOCom(entID);
	// apperently I forgot to delete animation components... time to fix that...
	for (auto iter = entList.begin(); iter != entList.end(); iter++) {
		if (*iter == entID) { return entList.erase(iter); }
	}
	printf("This should be impossible: delEntID()\n");
	return entList.end();
}

int makePlayer() {
	int entity = getEntID();

	SDL_Point *pts = (SDL_Point *)calloc(shipLen, sizeof(SDL_Point));
	memcpy(pts, shipPoints, sizeof(SDL_Point) * shipLen);
	addPointsCom(entity, pts, shipLen);

	addLocationCom(entity, v2(REN_WIDTH / 2, REN_HEIGHT / 2), RAD(-90));

	addMovementCom(entity, v2(0, 0), v2(0, 0));

	v2Pair *colPoints = (v2Pair *)calloc(shipLen - 1, sizeof(v2Pair));
	for (int i = 0; i < shipLen - 1; i++) {
		colPoints[i].one = shipPoints[i];
		colPoints[i].two = shipPoints[i + 1];
	}
	addColliderCom(entity, colPoints, shipLen - 1, 0, false);

	addShooterCom(entity, 4);

	return entity;
}

/**
 * will always have no rotation in its locationCom
 */
int makeBullet(v2 position, v2 velocity, int group, int owner) {
	comShooter *shooter = getAllComs(owner).shr;
	if (!shooter || shooter->curBullets >= shooter->maxBullets) {
		// return early if this entity cannot fire more bullets
		// or this entity has already died.
		return 0;
	}

	int entity = getEntID();

	addLocationCom(entity, position, RAD(0));

	addRectangleCom(entity, v2(3, 3));

	addMovementCom(entity, velocity, v2(0, 0));

	v2Pair *buffer = (v2Pair *)calloc(1, sizeof(v2Pair));
	addColliderCom(entity, buffer, 1, group, true);

	addLifetimeCom(entity, 0.016 * 75);

	addShotCom(entity, owner);

	return entity;
}

int makeBullet(BulletSpawnEvent e) { return makeBullet(e.pos, e.vel, e.group, e.owner); }

#define AST_MAXPTS (25)
int makeAsteroid(v2 position, int size) {
	// create Points data for asters
	// This data is at most AST_MAXPTS long. It can be as short as it needs to be.
	SDL_Point *pts = (SDL_Point *)calloc(AST_MAXPTS, sizeof(SDL_Point));
	int index = 0;
	int var = 4 * (size / 10);
	for (double ang = 0; ang <= 360; ang += getRandDouble(10, 60)) {
		double radius = (double)size;
		pts[index] = {(int)((radius + getRandDouble(-var, var)) * cos(RAD(ang))),
		              (int)((radius + getRandDouble(-var, var)) * sin(RAD(ang)))};
		index++;
		if (index == AST_MAXPTS - 1) { break; }
	}
	pts[index] = pts[0];

	int ptsSize = index + 1;
	pts = (SDL_Point *)realloc(pts, sizeof(SDL_Point) * ptsSize);

	// after points data is generated, start doing normal entity init.
	int entity = getEntID();

	addPointsCom(entity, pts, ptsSize);

	v2Pair *ptpairs = (v2Pair *)calloc(ptsSize - 1, sizeof(v2Pair));
	for (int i = 0; i < ptsSize - 1; i++) {
		ptpairs[i].one = {(double)pts[i].x, (double)pts[i].y};
		ptpairs[i].two = {(double)pts[i + 1].x, (double)pts[i + 1].y};
	}
	addColliderCom(entity, ptpairs, ptsSize - 1, 1, false);

	addLocationCom(entity, position, RAD(getRandDouble(0, 360)));

	double ang = getRandDouble(0, M_PI * 2);
	mat2 rotation ROT_MAT2(ang);
	double speed = 100.0 + (30.0 + state.speedmod / 2.0) * (30.0 / ((double)size));
	printf("speed: %f, size: %d\n", speed, size);
	v2 velocity = rotation * v2(speed, 0);
	addMovementCom(entity, velocity, v2(0, 0));

	addAsteroidCom(entity, size);
	return entity;
}

int makeUFO(int size) {
	int entity = getEntID();

	SDL_Point *pts = 0;
	v2Pair *cols = 0;
	if (size == UFO_BIG) {
		pts = (SDL_Point *)calloc(SizeOfArray(UFOPointsBig), sizeof SDL_Point);
		memcpy(pts, UFOPointsBig, sizeof SDL_Point * SizeOfArray(UFOPointsBig));
		addPointsCom(entity, pts, SizeOfArray(UFOPointsBig));

		cols = (v2Pair *)calloc(SizeOfArray(UFOColsBig), sizeof v2Pair);
		memcpy(cols, UFOColsBig, sizeof v2Pair * SizeOfArray(UFOColsBig));
		addColliderCom(entity, cols, SizeOfArray(UFOColsBig), 2, false);
	} else {
		pts = (SDL_Point *)calloc(SizeOfArray(UFOPointsSmall), sizeof SDL_Point);
		memcpy(pts, UFOPointsSmall, sizeof SDL_Point * SizeOfArray(UFOPointsSmall));
		addPointsCom(entity, pts, SizeOfArray(UFOPointsSmall));

		cols = (v2Pair *)calloc(SizeOfArray(UFOColsSmall), sizeof v2Pair);
		memcpy(cols, UFOColsSmall, sizeof v2Pair * SizeOfArray(UFOColsSmall));
		addColliderCom(entity, cols, SizeOfArray(UFOColsSmall), 2, false);
	}

	addShooterCom(entity, 1);

	int dir;
	if (getRandBool()) { // left side
		addLocationCom(entity, v2(0, getRandUint(REN_HEIGHT * 0.5, REN_HEIGHT * 0.95)), 0);
		addMovementCom(entity, v2(UFO_H_SPEED, 0), v2(0, 0));
		dir = UFO_RIGHT;
	} else { // right side
		addLocationCom(entity, v2(REN_WIDTH, getRandUint(REN_HEIGHT * 0.5, REN_HEIGHT * 0.95)), 0);
		addMovementCom(entity, v2(-UFO_H_SPEED, 0), v2(0, 0));
		dir = UFO_LEFT;
	}

	addUFOCom(entity, size, comLocMap.at(entity).pos, dir);

	return entity;
}

inline void makeDebrisRect(v2 pos, double ang) {
	int entity = getEntID();

	addLocationCom(entity, pos, 0);

	ang += getRandDouble(RAD(-5), RAD(5));
	mat2 rotationMat ROT_MAT2(ang);
	v2 vel(0, getRandDouble(30, 100));
	vel = rotationMat * vel;
	addMovementCom(entity, vel, v2(0, 0));

	addLifetimeCom(entity, 1.50); // TODO: should this be random?

	addRectangleCom(entity, v2(1, 1));
}

void makeDebrisLine(v2 pos, double rot, SDL_Point p1, SDL_Point p2) {
	int entity = getEntID();

	addLocationCom(entity, pos, rot);

	double ang = getRandDouble(RAD(0), RAD(360));
	mat2 rotationMat ROT_MAT2(ang);
	v2 vel(0, getRandDouble(10, 60));
	vel = rotationMat * vel;
	addMovementCom(entity, vel, v2(0, 0), getRandDouble(RAD(30), RAD(60)), 0);

	addLifetimeCom(entity, 1.60);

	comPoints *temp = (comPoints *)calloc(100, sizeof comPoints);
	for (int i = 0; i < 100; i++) {
		temp[i].pts = (SDL_Point *)calloc(2, sizeof SDL_Point);
		temp[i].pts[0] = p1;
		temp[i].pts[1] = p2;
		temp[i].len = 2;
		temp[i].pts[0].x = (int)(((double)(i - 100) / 100.0) * (double)temp[i].pts[0].x);
		temp[i].pts[0].y = (int)(((double)(i - 100) / 100.0) * (double)temp[i].pts[0].y);
		temp[i].pts[1].x = (int)(((double)(i - 100) / 100.0) * (double)temp[i].pts[1].x);
		temp[i].pts[1].y = (int)(((double)(i - 100) / 100.0) * (double)temp[i].pts[1].y);
	}
	addAnimationCom(entity, temp, 100, GAME_TICK_RATE);
}

void makeDebris(comLocation *loc, comPoints *poi, double radius, int amount) {
	int var = 4;
	int processed = 1;
	// TODO: improve this function
	// make a bunch of rects
	for (double ang = 0; ang <= 360; ang += getRandDouble(10, 20)) {
		v2 pos(((radius + getRandDouble(-var, var)) * cos(RAD(ang))),
		       ((radius + getRandDouble(-var, var)) * sin(RAD(ang))));
		pos += loc->pos;
		makeDebrisRect(pos, ang);

		processed++;
		if (processed > amount) { break; }
	}

	// make the lines fall apart
	if (poi) {
		for (int i = 0; i < poi->len - 1; i++) { makeDebrisLine(loc->pos, loc->rot, poi->pts[i], poi->pts[i + 1]); }
	}
}

int makeString(char *string, bool isConst, v2 pos) {
	int entity = getEntID();

	addTextCom(entity, string, isConst);
	addLocationCom(entity, pos, 0);

	return entity;
}
