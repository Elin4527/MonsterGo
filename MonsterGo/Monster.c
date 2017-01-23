#include "Monster.h"
#include "Resources.h"
#include <stdlib.h>

static const char* names[5] = {"CYCLOPS", "OCTOPUS", "OPAMP", "RESISTOR", "MUSHROOM"};

uint8_t IDLE = 0;
uint8_t MAD = 1;
uint8_t SAD = 2;
uint8_t DEAD = 3;

struct MonsterType* monsterIndex[5];

struct MonsterType
{
  struct Animation* a[4];
  const char* name;
  float baseHealth;
  float baseSpeed;
  float basePower;
  float healthGrowth;
  float speedGrowth;
  float powerGrowth;
};

struct MonsterType* createMonsterType(char idle1[2][128], char idle2[2][128], char mad1[2][128], char mad2[2][128],
  char sad1[2][128], char sad2[2][128], char dead[2][128], const char* name, float h, float s, float p, float hg, float sg, float pg)
{
  struct MonsterType* m = malloc(sizeof(struct MonsterType));
  if(!m) return NULL;
  m->a[0] = addFrame(animCreate(32, 32, 500, idle1[0], idle1[1]), idle2[0], idle2[1]);
  m->a[1] = addFrame(animCreate(32, 32, 500, mad1[0], mad1[1]), mad2[0], mad2[1]);
  m->a[2] = addFrame(animCreate(32, 32, 500, sad1[0], sad1[1]), sad2[0], sad2[1]);
  m->a[3] = animCreate(32, 32, 500, dead[0], dead[1]);
  m->name = name;
  m->baseHealth = h;
  m->baseSpeed = s;
  m->basePower = p;
  m->healthGrowth = hg;
  m->speedGrowth = sg;
  m->powerGrowth = pg;

  return m;
}

void initMonsterTypes()
{
  monsterIndex[0] = createMonsterType(cyclops_idle1, cyclops_idle2, cyclops_angry1, cyclops_angry2,
                    cyclops_sad1, cyclops_sad2, cyclops_dead,
                    "CYCLOPS", 15, 3, 5, 3.0, 1.95, 1.2); // P+
  monsterIndex[1] = createMonsterType(octopus_idle1, octopus_idle2, octopus_angry1, octopus_angry2,
                    octopus_sad1, octopus_sad2, octopus_dead,
                    "OCTOPUS", 15, 2, 5, 3.2, 2.1, 0.9); // H+, S+, P-
  monsterIndex[2] = createMonsterType(opamp_idle1, opamp_idle2, opamp_angry1, opamp_angry2,
                    opamp_sad1, opamp_sad2, opamp_dead,
                    "OPAMP", 14, 3, 5, 2.6, 2.1, 1.1); // S+, P+, H-
  monsterIndex[3] = createMonsterType(resistor_idle1, resistor_idle2, resistor_angry1, resistor_angry2,
                    resistor_sad1, resistor_sad2, resistor_dead,
                    "RESISTOR", 16, 2, 4, 3.5, 2.0, 1.0); // H+
  monsterIndex[4] = createMonsterType(mushroom_idle1, mushroom_idle2, mushroom_angry1, mushroom_angry2,
                    mushroom_sad1, mushroom_sad2, mushroom_dead,
                    "MUSHROOM", 15, 3, 4, 3.2, 1.9, 1.1); // H+, P+, S-
}

struct Monster* loadMonster(struct MonsterSave* s)
{
  struct Monster *m = malloc(sizeof(struct Monster));

  m->type = monsterIndex[s->index];
  m->state = s->state;
  m->a = animCopy(m->type->a[m->state]);
  m->level = s->level;
  m->friendship = s->friendship;
  m->tired = s->tired;
  m->hunger = s->hunger;
  m->health = s->health;
  m->maxHealth = s->maxHealth;
  m->speed = s->speed;
  m->power = s->power;

  return m;
}
struct MonsterSave* saveMonster(struct Monster* m)
{
  struct MonsterSave* s = malloc(sizeof(struct MonsterSave));
  for(uint8_t i = 0; i < 5; i++)
  {
    if(m->type == monsterIndex[i])
    { 
      s->index = i;
      break;
    }
  }
  s->state = m->state;
  s->level = m->level;
  s->friendship = m->friendship;
  s->tired = m->tired;
  s->hunger = m->hunger;
  s->health = m->health;
  s->maxHealth = m->maxHealth;
  s->speed = m->speed;
  s->power = m->power;

  return s;
}

static void updateStats(struct Monster* m)
{
  m->maxHealth = m->health = m->type->baseHealth + m->level * m->type->healthGrowth;
  m->speed = m->type->baseSpeed + m->level * m->type->speedGrowth;
  m->power = m->type->basePower + m->level * m->type->powerGrowth;
}

struct Monster* createMonster(struct MonsterType* t, uint8_t level, uint8_t state)
{
  struct Monster *m = malloc(sizeof(struct Monster));
  m->type = t;
  m->level = level;
  m->state = state;
  m->a = animCopy(t->a[state]);
  updateStats(m);
  m->friendship = 50.0; // can be increased by playing
  m->tired = 50.0; // increase during battle decrease when take a bath
  m->hunger = 50.0; // 0-not hungry 100 - starve to death

  return m;
}
struct Monster* levelUp(struct Monster* m)
{
  if(m && m->level < 100)
  {
    m->level++;
    m->maxHealth += m->type->healthGrowth + (m->friendship - 50.0) / 50.0;
    m->health = (int)m->maxHealth;
    m->power += m->type->powerGrowth + (50.0 - m->hunger) / 100.0;
    m->speed += m->type->speedGrowth + (50.0 - m->tired) /100.0;
  }
  return m;
}
struct Monster* setState(struct Monster* m, int state)
{
  if(m && m->state != state)
  {
    m->state = state;
    m->a = animFree(m->a);
    m->a = animCopy(m->type->a[state]);
  }
  return m;
}

struct Monster* updateState(struct Monster* m)
{
  if(m->health > 0)
  {
    if(m->friendship <= 25 || m->tired >= 75 || m->hunger >= 75) setState(m, SAD);
    else setState(m, IDLE);
  }
  else setState(m, DEAD);
}

int monsterAttack(struct Monster* attack, struct Monster* hit)
{
  int speedDiff = attack->speed - hit->speed;
  // Speed is a multiplier of damage if faster
  int damage = (int)(attack->power * ((speedDiff > 0)? (1 + (speedDiff / 100.0)) : 1));
  int ret = 0;
  
  // Check for crit or miss

  int rng = rand() % 100;
  if(rng >= 90)
  {
    return -1;
  }
  if(rng < 10)
  {
    ret = 1;
    damage *= 2;
  }

  hit->health -= damage;
  if(hit->health <= 0)
  {
    hit->health = 0;
    setState(hit, DEAD);
  }
  return ret;
}

char* getName(struct Monster* m)
{
  return m->type->name;
}

struct Monster* freeMonster(struct Monster* m)
{
  if(m)
  {
    animFree(m->a);
    free(m);
  }
  return NULL;
}
