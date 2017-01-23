#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdint.h>

void gameInit();
void processFrame(uint64_t time);
void drawFrame(uint64_t time);

#endif
