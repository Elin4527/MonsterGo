#ifndef OVERWORLD_H
#define OVERWORLD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Animation.h"

  extern const float FOUND;
  extern const float CLOSE;
  extern const float FAR;

  void initOverworld();
  void drawWorld(uint64_t t);
  void movePos(float delta);
  void nextWorld();
  void prevWorld();
  float getProximity();
  uint8_t getMonsterIndex();

#ifdef __cplusplus
}
#endif

#endif
