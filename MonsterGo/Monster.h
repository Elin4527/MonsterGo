#ifndef MONSTER_H
#define MONSTER_H

#ifdef __cplusplus
extern "C"{
#endif


#include "Energia.h"
#include "Animation.h"
#include <delay.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>

  extern uint8_t MAD;
  extern uint8_t SAD;
  extern uint8_t IDLE;
  extern uint8_t DEAD;

  struct MonsterType;

  // Complete List of Monster Types
  extern struct MonsterType* monsterIndex[5];

  // Pack the struct so the compiler does not pad
  // Important for reading and writing binary
#pragma pack(push, 1)
  struct MonsterSave
  {
    float maxHealth;
    float power;
    float speed;
    
    int16_t health;
    uint8_t level;
    uint8_t state;
    
    uint8_t friendship;
    uint8_t tired;
    uint8_t hunger;
    
    uint8_t index;
  };
#pragma pack(pop)

  struct Monster{
    struct Animation* a;
    struct MonsterType* type;
    uint8_t state;
    uint8_t level;
    int16_t health;
    float maxHealth;
    float power;
    float speed;

    // Tamagotchi type stats that range from 0 - 100
    uint8_t friendship; // 100 Good
    uint8_t hunger;     // 0 Good
    uint8_t tired;      // 0 Good
  };

  void initMonsterTypes();

  struct Monster* createMonster(struct MonsterType* m, uint8_t level, uint8_t state);
  struct Monster* loadMonster(struct MonsterSave* s);
  struct MonsterSave* saveMonster(struct Monster* m);

  char* getName(struct Monster* m);
  
  struct Monster* levelUp(struct Monster* m);
  struct Monster* setState(struct Monster* m, int state);
  struct Monster* updateState(struct Monster* m);
  struct Monster* freeMonster(struct Monster* m);
  
  // Return <0 for miss, 0 for hit, >0 for Crit
  int monsterAttack(struct Monster* attack, struct Monster* hit);
#ifdef __cplusplus
}
#endif

#endif
