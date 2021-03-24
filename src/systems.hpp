#pragma once
#include "components.hpp"

void renderPoints(const SDL_Point *, const size_t, const v2, double);
void renderPoints(const comPoints *, const comLocation *);

void renderRectangle(const comLocation *, const comRectangle *);

void renderColliders(const comLocation *, const comCollider *);

void renderString(const comLocation *, const char *);
void renderString(const comLocation *, const comText *);

void advanceAnimation(comAnimation *, const comLocation *, double);

void moveEntity(comMovement *, comLocation *, double);

void checkCollide(const comCollider *, const comLocation *, int);

void generateColliders(const comCollider *, const comMovement *, double);

bool advanceLifetime(comLifetime *, double);

bool doUFOai(const int, comUFO *, comMovement *, comLocation *, const double, const comLocation *);
