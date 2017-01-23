#include "Animation.h"
#include <stdlib.h>

const uint8_t NORMAL = 0;
const uint8_t MASK = 1;

struct Animation{
  uint64_t updateTime;
  uint64_t delay;
  uint8_t len;
  uint8_t currFrame;
  uint8_t width;
  uint8_t height;
  char** data;
};

struct Animation* animCreate(uint8_t x, uint8_t y,  uint64_t fr, char *n, char *m)
{
  struct Animation *a = malloc(sizeof(struct Animation));
  if(!a) return NULL;
  a->updateTime = fr;
  a->delay = fr;
  a->width = x;
  a->height = y;
  a->len = 1;
  a->currFrame = 0;
  // 2 frames per animation frame (NOMAL and MASK)
  a->data = (char**)malloc(sizeof(char*) * 2);
  a->data[0] = n;
  a->data[1] = m;
  return a;
}

struct Animation* animCopy(struct Animation* a)
{
  struct Animation* n = malloc(sizeof(struct Animation));
  n->updateTime = a->updateTime;
  n->delay = a->delay;
  n->len = a->len;
  n->width = a->width;
  n->height = a->height;
  n->currFrame = 0;

  // Allocate new memory for pointers during copy
  n->data = (char**)malloc(sizeof(char*) * n->len * 2);
  for(int i = 0; i < n->len * 2; i++)
  {
    n->data[i] = a->data[i];
  }

  return n;
}

struct Animation* addFrame(struct Animation* a, char* n, char* m)
{
  a->len++;
  a->data = (char**)realloc(a->data, sizeof(char*) * a->len * 2);
  a->data[(a->len-1)  * 2] = n;
  a->data[(a->len-1)  * 2 + 1] = m;
  return a;
}

struct Animation* animUpdate(struct Animation* a, uint64_t time)
{
  // Catch up immediately rather than showing fast frames
  // when not updated for a while
  while(time >= a->updateTime)
  {
    if(++(a->currFrame) >= a->len) a->currFrame = 0;
    a->updateTime += a->delay;
  }
}

struct Animation* animFree(struct Animation* a)
{
  if(a)
  {
    free(a->data);
    free(a);
  }
  return NULL;
}

char* animFrame(struct Animation* a, uint8_t layer)
{
  return a->data[(a->currFrame * 2) + layer];
}

int animWidth(struct Animation* a)
{
  return a->width;
}

int animHeight(struct Animation* a)
{
  return a->height;
}
