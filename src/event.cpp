#include "event.hpp"

#include "entity.hpp"
#include "main.hpp"

std::vector<CollisionEvent> colEveList;
std::vector<BulletSpawnEvent> bulSpawnEveList;

void addCollisionEvent(int entID, int causeEntID) {
	CollisionEvent event = {};

	for (auto iter = colEveList.begin(); iter != colEveList.end(); iter++) {
		if (iter->entID == entID) {
			return; // avoid logging duplicate collisions
		}
	}

	event.entID = entID;

	AllComs causeComs = getAllComs(causeEntID);
	if (causeComs.ast) { event.cause = ASTER; }
	if (causeComs.sht) {
		event.cause = BULLET;
		event.bulletOwner = causeComs.sht->owner;
	}
	if (causeComs.ufo) { event.cause = UFO; }
	if (causeEntID == state.playerEntID) { event.cause = PLAYER; }

	colEveList.push_back(event);
}

// TODO: for some reason there is a bogus collision even that happens when the player dies? This could cause werid
// things to happen if we don't fix it, even if the chances of it are low. after some testing, I'm not sure if this
// actually happens?
void processCollisionEvent(CollisionEvent event) {
	if (event.entID == state.playerEntID) {
		AllComs coms = getAllComs(state.playerEntID);
		makeDebris(coms.loc, coms.poi, 10, 5);
		delEntID(event.entID);
		state.playerEntID = -1;
		if (state.lives == 0) {
			state.state = GAME_OVER;
		} else {
			state.state = RESPAWN_COOLDOWN;
		}
		state.timer = 0;
	} else if (comShtMap.count(event.entID)) {
		delEntID(event.entID);
	} else if (comAstMap.count(event.entID)) {
		int size = comAstMap.at(event.entID).size;
		AllComs coms = getAllComs(event.entID);
		if (event.cause == BULLET && event.bulletOwner == state.playerEntID) {
			if (size >= 30) { // less points the bigger it is
				state.score += 20 - 1.0 * (size - 30);
			} else if (size >= 19) { // less points the bigger it is
				state.score += 50 - 4.8 * (size - 20);
			} else { // more points the smaller it is
				state.score += 100 + 11.9 * (10 - size);
			}
		}
		size -= 9;
		v2 pos = comLocMap.at(event.entID).pos;
		if (size > 9) {
			makeAsteroid(pos, size);
			makeAsteroid(pos, size);
		}
		makeDebris(coms.loc, coms.poi, (double)coms.ast->size, 5); // TODO: do we want to pass poi here?
		makeDebris(coms.loc, 0, (double)coms.ast->size, 5);
		delEntID(event.entID);
	} else if (comUFOMap.count(event.entID)) {
		comUFO com = comUFOMap.at(event.entID);
		AllComs coms = getAllComs(event.entID);
		if (event.cause == BULLET && event.bulletOwner == state.playerEntID) {
			if (com.size == UFO_BIG) {
				state.score += 250;
				makeDebris(coms.loc, coms.poi, 6, 5);
			} else {
				state.score += 500;
				makeDebris(coms.loc, coms.poi, 10, 7);
			}
		}
		delEntID(event.entID);
	}
}

void addBulSpawnEvent(v2 ipos, v2 ivel, int igroup, int iowner) {
	BulletSpawnEvent temp = {ipos, ivel, igroup, iowner};
	bulSpawnEveList.push_back(temp);
}

void processBulletSpawnEvent(BulletSpawnEvent event) { makeBullet(event); }