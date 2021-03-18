#include "systems.hpp"

#include <stdexcept>
#include "math.hpp"
#include "main.hpp"
#include "text.txt"
#include "entity.hpp"
#include "RNG.hpp"

/**
 * renders line segments that make up an image
 * depends on comPoints and comLocation
 */
void renderPoints (const SDL_Point *pts, const size_t len, const v2 pos, double rot) {
	SDL_Point* pt = (SDL_Point*) calloc (len, sizeof(SDL_Point));
    
	for (int i = 0; i < len; i++) {
		v2 temp (pts[i].x, pts[i].y);
		mat2 rotation ROT_MAT2(rot);
		temp = rotation * temp;
		pt[i].x = (int)(temp.x + pos.x);
		pt[i].y = (int)(temp.y + pos.y);
	}
	SDL_RenderDrawLines (ren, pt, len);
	free (pt);
}

void renderPoints (const comPoints *poi, const comLocation *loc) {
    if (poi == 0 || loc == 0) {
		return;
	}
    renderPoints (poi->pts, poi->len, loc->pos, loc->rot);
}

/**
 * draws a rectangle
 * depends on comLocation, comRectangle.
 */
void renderRectangle (const comLocation *loc, const comRectangle *rec) {
	if (loc == 0 || rec == 0) {
		return;
	}
	SDL_Rect rect = {(int)loc->pos.x, (int)loc->pos.y, (int)rec->dim.x, (int)rec->dim.y};
	SDL_RenderDrawRect (ren, &rect);
	SDL_RenderFillRect (ren, &rect);
}

/**
 * renders the line segments of colliders
 * depends on comCollide and comLocation
 */
void renderColliders (const comLocation *loc, const comCollider *col) {
	if (loc == 0 || col == 0) {
		return;
	}
    
	for (int i = 0; i < col->len; i++) {
		mat2 rotation ROT_MAT2(loc->rot);
		v2Pair pair = col->ptPairs[i];
		pair.one = rotation * pair.one + loc->pos;
		pair.two = rotation * pair.two + loc->pos;
		SDL_RenderDrawLine (ren,
		                    (int)pair.one.x, (int)pair.one.y,
		                    (int)pair.two.x, (int)pair.two.y);
	}
}

void renderString (const comLocation *loc, const char *txt) {
	if (!loc || !txt) {
		return;
	}
    
	comLocation temp = *loc;
	for (int i = 0; txt[i] != 0; i++) {
		//Draw character data.
		temp.pos += v2(3, 5); //offset, accounts for center of character
		if (textPointComs[txt[i]].len != 0)
            renderPoints ((comPoints*)&textPointComs[txt[i]], &temp);
		temp.pos -= v2(3, 5);
		//Add inter-character spacing
		temp.pos.x += 8;
		//Next Character!
	}
}

void renderString (const comLocation *loc, const comText *txt) {
    if (!txt) {
        return; //we check *loc and txt->str later.
    }
    renderString(loc, txt->str);
}

/**
Draws animation data then advances to the next frame
*/
void advanceAnimation (comAnimation *ani, const comLocation *loc, double delta) {
    if (!ani || !loc) {
        return;
    }
    
    renderPoints(&ani->images[ani->curImage - 1], loc);
    
    ani->elapsedTime += delta;
    while (ani->elapsedTime > ani->rate) {
        ani->curImage++;
        if (ani->curImage > ani->totalImages) {
            ani->curImage = 1;
        }
        ani->elapsedTime -= ani->rate;
    }
}
/**
 * translates an entity according to it's movment com
 * depends on comMovement, comLocation.
* despawns UFOS if they go off the edge of the screen in either X direction.
* @return true if entity needs to despawn itself.
 */
void moveEntity (comMovement *mov, comLocation *loc, double delta) {
	if (!mov || !loc) {
		return;
	}
    
	//classic kinematic eqs
	loc->pos += mov->vel * delta + mov->acl * delta * delta * 0.5;
	mov->vel += mov->acl * delta;
    
    loc->rot += mov->angVel * delta + mov->angAcl * delta * delta * 0.5;
    mov->angVel += mov->angAcl * delta;
    
	if (mov->vel.getMag() > 500) {
		mov->vel = mov->vel.getUnit() * 500;
		mov->acl = {0, 0};
	}
    
	//wrap arounds
	if (loc->pos.x > REN_WIDTH) {
		loc->pos.x -= REN_WIDTH;
	}
	if (loc->pos.y > REN_HEIGHT) {
		loc->pos.y -= REN_HEIGHT;
	}
	if (loc->pos.x < 0) {
		loc->pos.x += REN_WIDTH;
	}
	if (loc->pos.y < 0) {
		loc->pos.y += REN_HEIGHT;
	}
}

/**
 * Checkes to see if 2 given collider v2Pair's are within
 * each other's bounding box
 * @returns true if the bounding boxs are intersecting
 */
bool checkColliderBB (v2Pair a, v2Pair b) {
	//"Normal-ize" Bounding Boxes,
	//they must have an upper left "location" and a "dimention"
	if (a.one.x - a.two.x > 0) {
		double temp = a.one.x;
		a.one.x = a.two.x;
		a.two.x = temp;
	}
	if (a.one.y - a.two.y > 0) {
		double temp = a.one.y;
		a.one.y = a.two.y;
		a.two.y = temp;
	}
    
	if (b.one.x - b.two.x > 0) {
		double temp = b.one.x;
		b.one.x = b.two.x;
		b.two.x = temp;
	}
	if (b.one.y - b.two.y > 0) {
		double temp = b.one.y;
		b.one.y = b.two.y;
		b.two.y = temp;
	}
    
	//preform check
	if ( ((a.two.x > b.one.x)&&(a.two.y > b.one.y)) &&
        ((a.one.x < b.two.x)&&(a.one.y < b.two.y)) ) {
		return true;
	}
	return false;
}

/**
 * Finds collisions between line Segments in unlike groups.
 * depends on comCollider and comLocation
 */
void checkCollide (const comCollider *col, const comLocation *loc, const int entID) {
	if (!col || !loc) {
		return;
	}
    
    //TODO: This has bad big-O. need to fix this.
    //for each entity with a colision com, we loop though all active entities.
    //deberis makes this function's preformance cost even worse than it
    //normally is.
    
    //should we just section off deberis from normal entities?
    //or should we section off everything with a collider com?
    //Both sound like decent options... I think I like the 2nd one better.
	
    //get list of comColliders that belong to this entity
    for (auto iter = entList.begin(); iter != entList.end(); iter++) {
		try {
			comCollider otherCol = comColMap.at(*iter);
			comLocation otherLoc = comLocMap.at(*iter);
			if (otherCol.grp != col->grp) {
				for (int i = 0; i < col->len; i++) {
					for (int j = 0; j < otherCol.len; j++) {
						mat2 rotation ROT_MAT2(loc->rot);
						v2Pair a = { (rotation * col->ptPairs[i].one) + loc->pos, (rotation * col->ptPairs[i].two) + loc->pos };
						rotation = mat2 ROT_MAT2(otherLoc.rot);
						v2Pair b = { (rotation * otherCol.ptPairs[j].one) + otherLoc.pos, (rotation * otherCol.ptPairs[j].two) + otherLoc.pos };
						if (checkColliderBB (a, b)) {
							//matrix math time
							mat2 foo (a.two.x - a.one.x, b.one.x - b.two.x,
							          a.two.y - a.one.y, b.one.y - b.two.y);
							if (foo.determinant () == 0) {
								//need to do alt test because these lines are parallel.
								if (abs(v2(a.two - a.one).dot(b.one - a.one)) == (b.one - a.one).getMag()) {
									addCollisionEvent (entID, *iter);
									addCollisionEvent (*iter, entID);
									printf ("parallel collision detected!\n");
									return;
								}
							}
							foo = foo.inverse();
							v2 bar (b.one.x - a.one.x, b.one.y - a.one.y);
							v2 result = foo * bar;
							if ((result.x < 1.0 && result.x > 0.0) && (result.y < 1.0 && result.y > 0.0)) {
								addCollisionEvent (entID, *iter);
								addCollisionEvent (*iter, entID);
								return;
							}
							//maybe if we log collision we can stop looking for one?
						}
					}
				}
			}
		} catch (std::out_of_range e) {}
	}
}

void generateColliders (const comCollider *col, const comMovement *mov, const double delta) {
	if (!col || !mov || !col->depsOnMov) {
		return;
	}
    
	v2Pair newpts = {};
	newpts.two = {0, 0}; //new "location" is center point
	v2 displacement = mov->vel * delta + mov->acl * delta * delta * 0.5;
	newpts.one = -displacement;
	memcpy (col->ptPairs, &newpts, sizeof(v2Pair));
}

/**
 * advances the lifetime of entities that posses it.
 * depends on comLifetime
 * Lifetime is measured in frames, so that's why it doesn't need delta time.
 */
bool advanceLifetime (comLifetime *lif, const double delta) {
	if (!lif) {
		return false;
	}
    
	lif->elapsed += delta;
	if (lif->elapsed > lif->length) {
		return true;
	}
	return false;
}

/*
ufos seem to start on a random side of the play field and move to the other side.
Big's shoot randomly, Small's shoot aimed at the player with random deviation.
They shoot about once per second.
while moving, the randomly decide to strafe up or down. smaller ufos do this more often.
*/

/**
* @return if the UFO should despawn
*/
bool doUFOai (const int entID, comUFO *ufo, comMovement *mov, comLocation *loc, const double delta, const comLocation *playerLoc) {
    if (!(ufo && mov && loc)) { return false; }
    
    //manage random motion
    if (ufo->timer > ufo->nextTurn) {
        ufo->timer = 0;
        if (ufo->turnState == UFO_STRAIGHT) {
            ufo->nextTurn = getRandDouble (0.5, 3.0);
            if (getRandBool()) {
                mov->vel.y = 75;
            }
            else {
                mov->vel.y = -75;
            }
            ufo->turnState = UFO_DIAGONAL;
        }
        else {
            ufo->nextTurn = getRandDouble (1.5, 5.5);
            mov->vel.y = 0;
        }
    }
    ufo->timer += delta;
    
    //kill self if we wrapped around
    if (ufo->direction == UFO_LEFT) {
        if (ufo->lastPos.x < loc->pos.x) {
            return true;
        }
    }
    else {
        if (ufo->lastPos.x > loc->pos.x) {
            return true;
        }
    }
    
    if (ufo->size == UFO_BIG) {
        double ang = getRandDouble(0, RAD(360));
        mat2 rotMat ROT_MAT2(ang);
        v2 bulVel = rotMat * v2(400, 0);
        addBulSpawnEvent(loc->pos, bulVel, 2, entID);
    }
    else if (playerLoc && ufo->size == UFO_SMALL) {
        double ang = (-(loc->pos - playerLoc->pos)).getAngle();
        mat2 rotMat ROT_MAT2(ang);
        v2 bulVel = rotMat * v2(400, 0);
        addBulSpawnEvent(loc->pos, bulVel, 2, entID);
    }
    
    
    ufo->lastPos = loc->pos;
    
    return false;
}