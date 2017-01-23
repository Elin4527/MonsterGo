#ifndef PLAYER_H
#define PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Monster.h"
#include "SaveFile.h"

  void createPlayer();

  // Functions to save and load games
  void loadPlayer(struct DataFile* f);
  struct DataFile* savePlayer();

  void useCapsule();
  void gainCapsule();
  void addMonster(struct Monster* m);
  void removeMonster(uint8_t i);
  void setActiveMonster(uint8_t i);
  
  uint8_t getPartySize();
  uint8_t getCapsules();
  uint8_t getPartyIndex();
  uint8_t getMaxPartySize();
  
  struct Animation* getPlayer();
  struct Monster* getActiveMonster();
  struct Monster* getMonster(uint8_t i);
  int nextAlive();
  int prevAlive();

#ifdef __cplusplus
}
#endif

#endif

