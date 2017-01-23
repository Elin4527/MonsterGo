#ifndef ANIMATION_H
#define ANIMATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

  extern const uint8_t NORMAL;
  extern const uint8_t MASK;

  struct Animation;

  struct Animation* animCreate(uint8_t x, uint8_t y,  uint64_t fr, char* n, char* m);
  struct Animation* animCopy(struct Animation* a);
  struct Animation* addFrame(struct Animation* a, char* n, char* m);
  struct Animation* animUpdate(struct Animation* a, uint64_t time);
  struct Animation* animFree(struct Animation* a);
  char* animFrame(struct Animation* a, uint8_t layer);
  int animWidth(struct Animation* a);
  int animHeight(struct Animation* a);


#ifdef __cplusplus 
} 
#endif 

#endif
