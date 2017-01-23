#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <delay.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>

#include <string.h>
#include "Animation.h"

// Global Text Buffer to use for sprintf
extern char TEXT_BUFF[30];

void graphicsInit();
void graphicsBegin();
void graphicsEnd();

void drawAnim(struct Animation* a, int x, int y);
// Only used to draw repeating textures that are 8 high
void drawTileTexture(struct Animation* a, int offx, int offy, int rows);
void drawTextAt(const char* test, int x, int y);
void drawText(const char* test);

#ifdef __cplusplus
}
#endif

#endif
