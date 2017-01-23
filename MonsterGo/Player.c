
#include <stdint.h>
#include "Player.h"
#include "Resources.h"


static struct Monster* party[5];
static const uint8_t MAX_PARTY = (uint8_t)(sizeof(party)/sizeof(party[0]));
static const uint8_t MAX_CAPSULES = 9;
static struct Animation* player = NULL;
static uint32_t partySize = 0;
static uint8_t partyIndex = 0;
static uint8_t capsules = 3;


void freeParty()
{
  for(int i = 0; i < partySize; i++)
  {
    freeMonster(party[i]);
  }
  partySize = 0;
  partyIndex = 0;
}

void loadPlayer(struct DataFile* f)
{
  if(!f) return;
  if(partySize)
  {
    freeParty();
  }
  else
  {
    player = addFrame(animCreate(32, 32, 500, player1[0], player1[1]), player2[0], player2[1]);
  }
  partySize = f->s;
  partyIndex = f->index;
  capsules = f->capsules;
  for(int i = 0; i < partySize; i++)
  {
    party[i] = loadMonster(&(f->monsterSave[i]));
  }
}

struct DataFile* savePlayer()
{
  struct DataFile *f = malloc(sizeof(struct DataFile));
  if(!f) return NULL;
  f->s = partySize;
  f->index = partyIndex;
  f->capsules = capsules;
  for(int i = 0; i < partySize; i++)
  {
    f->monsterSave[i] = *saveMonster(party[i]);
  }
  return f;
}

void createPlayer()
{
  if(partySize)
  {
    freeParty();
  }
  else
  {
    player = addFrame(animCreate(32, 32, 500, player1[0], player1[1]), player2[0], player2[1]);
  }
  
  // Start with a lvl 1 Cyclops
  partySize = 1;
  capsules = 3;
  party[0] = createMonster(monsterIndex[0], 1, IDLE);
}

uint8_t getPartySize()
{
  return  partySize;
}

uint8_t getCapsules()
{
  return capsules;
}

uint8_t getPartyIndex()
{
  return partyIndex;
}

uint8_t getMaxPartySize()
{
  return MAX_PARTY;
}

void useCapsule()
{
  if(capsules > 0) capsules--;
}

void gainCapsule()
{
  if(capsules < MAX_CAPSULES) capsules++;
}

void addMonster(struct Monster* m)
{
  if(partySize < MAX_PARTY && m)
  {
    party[partySize++] = m;
    updateState(m);
  }
}

void removeMonster(uint8_t i)
{
  if (i >= partySize || partySize <= 1) return;
  freeMonster(party[i]);
  while(++i < partySize)
  {
    party[i-1] = party[i];
  }
  partySize--;
}

struct Animation* getPlayer()
{
  return player;
}

struct Monster* getActiveMonster()
{
  return party[partyIndex];
}

void setActiveMonster(uint8_t i)
{
  if(i < partySize) partyIndex = i;
}

struct Monster* getMonster(uint8_t i)
{
  if (i >= partySize) return NULL;
  return party[i];
}

int nextAlive()
{
  int i = partyIndex + 1;
  if(i >= partySize) i = 0;
  while(i != partyIndex)
  {
    if(party[i]->state != DEAD) return i;
    if(++i >= partySize) i = 0;
  }
  return -1;
}
int prevAlive()
{
  int i = partyIndex - 1;
  if(i < 0) i = partySize - 1;
  while(i != partyIndex)
  { 
    if(party[i]->state != DEAD) return i;
    if(--i < 0) i = partySize - 1;
  }
  return -1;
}
