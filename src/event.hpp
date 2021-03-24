#pragma once
#include "components.hpp"
#include <vector>

enum {
	ASTER,
	PLAYER,
	BULLET,
	UFO,
};

struct CollisionEvent {
	int entID;
	int cause;
	int bulletOwner; // used only if cause is BULLET
};

extern std::vector<CollisionEvent> colEveList;

void addCollisionEvent(int, int);
void processCollisionEvent(CollisionEvent);

struct BulletSpawnEvent {
	v2 pos;
	v2 vel;
	int group;
	int owner;
};

extern std::vector<BulletSpawnEvent> bulSpawnEveList;

void addBulSpawnEvent(v2, v2, int, int);
void processBulletSpawnEvent(BulletSpawnEvent);