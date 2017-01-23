#ifndef TRAIN_H
#define TRAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Monster.h"
#include <stdbool.h>

enum TrainActions{
  PLAY, SLEEP, FEED, FREE 
};

void createTrain(uint64_t t);
void drawTrain(uint64_t t);
void exitTrain();

bool trainMenu();
bool selectingMonster();
void stopSelecting();
void startSelecting();
void startTraining(uint8_t a);

void setTrainMsg(const char* m);
void setTrainMsgDelay(const char* m, uint64_t time);

#ifdef __cplusplus
}
#endif

#endif

