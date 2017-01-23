#ifndef BATTLE_H
#define BATTLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Monster.h"
#include <stdbool.h>

  enum BattleActions{
    ATTACK, SWITCH, CATCH, RUN
  };

  void createBattle(struct MonsterType* m, uint8_t lvl, uint64_t t);
  void drawBattle(uint64_t t);
  void exitBattle();

  bool waitingForInput();
  bool isBattleDone();
  bool isSwitching();
  void startSwitching();
  void stopSwitching(int confirm);

  void processAction(uint8_t);

  void setBattleMsg(const char* m);
  void setBattleMsgDelay(const char* m, uint64_t time);

#ifdef __cplusplus
}
#endif

#endif

