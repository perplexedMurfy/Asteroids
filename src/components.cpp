#include <stdexcept>
#include "components.hpp"
#include "RNG.hpp"

//TODO: Add's and Del's could return bools regarding if they succeed or not.

/////COM LOCATION START
std::unordered_map<int, comLocation> comLocMap;

void addLocationCom (int entID, v2 position, double rotation) {
	comLocation com;
	com.pos = position;
	com.rot = rotation;
	comLocMap.insert_or_assign (entID, com);
}

void delLocationCom (int entID) {
	comLocMap.erase (entID);
}
/////COM LOCATION END

/////COM POINTS START
std::unordered_map<int, comPoints> comPoiMap;

/**
 * @param entID entity to bind to
 * @param p list of points that should be rendered
 *          Must be allocated.
 * @param pCount length of above
 */
void addPointsCom (int entID, SDL_Point* p, int pCount) {
	comPoints com;
	com.pts = p;
	com.len = pCount;
	comPoiMap.insert_or_assign (entID, com);
}

void delPointsCom (int entID) {
	try {
		comPoints com = comPoiMap.at(entID);
		free (com.pts);
		comPoiMap.erase (entID);
	} catch (std::out_of_range e) {}
}
/////COM POINTS END

/////COM MOVEMENT START
std::unordered_map<int, comMovement> comMovMap;

void addMovementCom (int entID, v2 velocity, v2 acceleration, double angularVel, double angularAcl) {
	comMovement com;
	com.vel = velocity;
	com.acl = acceleration;
    com.angVel = angularVel;
    com.angAcl = angularAcl;
	comMovMap.insert_or_assign (entID, com);
}

//TODO: DEPERCATED, remove
void addMovementCom (int entID, v2 vel, v2 acl) { //Inline?
    addMovementCom(entID, vel, acl, 0, 0);
}

void delMovementCom (int entID) {
	comMovMap.erase (entID);
}
/////COM MOVEMENT END

/////COM RECTANGLE START
std::unordered_map<int, comRectangle> comRecMap;

void addRectangleCom (int entID, v2 dimentions) {
	comRectangle com;
	com.dim = dimentions;
	comRecMap.insert_or_assign (entID, com);
}

void delRectangleCom (int entID) {
	comRecMap.erase (entID);
}
/////COM RECTANGLE END

/////COM COLLIDER START
std::unordered_map<int, comCollider> comColMap;

/**
 * @param entID id of entity to bind to.
 * @param colpts list of v2Pair's that represent all lines of colision that the obj has
 *               must be a allocated pointer.
 * @param length length of colpts
 * @param group denotes what group these colliders belong to.
 * @param dependsOnMovement sets depsOnMov;
 * If a collider has depsOnMov set, then colpts should be an
 * allocated as a array with len of 1. The collider will be updated according to the
 * entity's movement component. This setting is intended for use with projectiles.
 */
void addColliderCom (int entID, v2Pair* colpts, int length, int group, bool dependsOnMov) {
	comCollider com = {colpts, length, group, dependsOnMov};
	comColMap.insert_or_assign (entID, com);
}

void delColliderCom (int entID) {
	try {
		comCollider com = comColMap.at(entID);
		free (com.ptPairs);
		comColMap.erase (entID);
	} catch (std::out_of_range e) {}
}
/////COM COLLIDER END

/////COM ASTEROID START
std::unordered_map<int, comAsteroid> comAstMap;

void addAsteroidCom (int entID, int startSize) {
	comAsteroid com = {startSize};
	comAstMap.insert_or_assign (entID, com);
}

void delAsteroidCom (int entID) {
	comAstMap.erase (entID);
}
/////COM ASTEROID END

/////COM LIFETIME START
std::unordered_map<int, comLifetime> comLifMap;

/**
 * @param entID entity handle
 * @param len length in frames of how long this object should exist for
 */
void addLifetimeCom (int entID, double len) {
	comLifetime com = {len, 0};
	comLifMap.insert_or_assign (entID, com);
}

void delLifetimeCom (int entID) {
	comLifMap.erase(entID);
}
/////COM LIFETIME END

/////COM SHOOTER START
std::unordered_map<int, comShooter> comShrMap;

void addShooterCom (int entID, int max) {
	comShooter com = {max, 0};
	comShrMap.insert_or_assign (entID, com);
}

void delShooterCom (int entID) {
	comShrMap.erase (entID);
}
/////COM SHOOTER END

/////COM SHOT START
std::unordered_map<int, comShot> comShtMap;

void addShotCom (int entID, int ownerID) {
	comShot com = {ownerID};
	try {
		comShrMap.at (ownerID).curBullets++;
		comShtMap.insert_or_assign (entID, com);
	} catch (std::out_of_range e) {}
}


void delShotCom (int entID) {
	try {
		int owner = comShtMap.at (entID).owner;
		comShtMap.erase (entID);
		comShrMap.at (owner).curBullets--;
	} catch (std::out_of_range e) {}
}
/////COM SHOT END

/////COM TEXT START
std::unordered_map<int, comText> comTxtMap;

/**
 * @param string char array
* @param isConst if the char array is const or not.
*
* NOTE: the char array will be auto freed if isConst is false
 */
void addTextCom (int entID, char* string, bool isConst) {
	comText com {string, isConst};
	comTxtMap.insert_or_assign (entID, com);
}

/**
 * str param will be free'd
 */
void delTextCom (int entID) {
	try {
		comText com = comTxtMap.at (entID);
		if (!com.isConst)
            free (com.str);
		comTxtMap.erase (entID);
	}
	catch (std::out_of_range e) {
	}
}
/////COM TEXT END

/////COM UFO START
std::unordered_map<int, comUFO> comUFOMap;

void addUFOCom (int entID, int size, v2 lastPos, int dir) {
    comUFO com = {size, 0, getRandDouble(0.5, 5.5), UFO_STRAIGHT, lastPos, dir};
    
    comUFOMap.insert_or_assign (entID, com);
}

void delUFOCom (int entID) {
    try {
        comUFOMap.erase (entID);
    }
    catch (std::out_of_range e) {}
}
/////COM UFO END

/////COM ANIMATION START
std::unordered_map<int, comAnimation> comAniMap;

void addAnimationCom (int entID, comPoints *images, int totalImages, double rate) {
    comAnimation com = {images, totalImages, 1, rate, 0};
    
    comAniMap.insert_or_assign(entID, com);
}

void delAnimationCom (int entID) {
    try {
        comAnimation com = comAniMap.at(entID);
        for (int i = 0; i < com.totalImages; i++) {
            free(com.images[i].pts);
        }
        free(com.images);
        comAniMap.erase (entID);
    }
    
    catch (std::out_of_range e) {}
}
/////COM ANIMATION END
