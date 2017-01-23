#include "Battle.h"
#include "Animation.h"
#include "Player.h"
#include "Graphics.h"
#include "Resources.h"
#include "IOSystem.h"

// List of random backgrounds
static char (*bgIndex[4])[16] = {battle1, battle2, battle3, battle4};
static struct Animation* bg;
static struct Monster *foe, *team;

static uint64_t animTime = 1500;
static uint64_t timeUntil = 0;
static uint64_t currTime = 0;

static uint8_t action;

static bool playerTurn;
static bool done;
static bool switching;
static bool pending;
static bool caught;

static size_t switchIndex;

static const char* msg;

void createBattle(struct MonsterType* m, uint8_t lvl, uint64_t t)
{
  // Create random background for battle
  int rng = rand() %4;
  bg = animCreate(16, 8, 1000, bgIndex[rng][0], bgIndex[rng][1]);
  
  // Generate the enemy
  foe = createMonster(m, lvl, MAD);
  team = getActiveMonster();

  // Used to delay first action so not instantly attacked
  currTime = t;
  timeUntil = animTime + t;
  
  // Faster player goes first (priority to player)
  playerTurn = team->speed >= foe->speed;
  done = switching = caught = pending = false;

  msg = getName(foe);
}

void drawBattle(uint64_t t)
{
  // Update Team in case of switch
  currTime = t;
  team = getActiveMonster();

  // Actions executed after a delay
  // Allows messages to be displayed
  if(currTime >= timeUntil && pending)
  {
    executeAction();
  }
  
  // Process enemy turn on a drawUpdate if delay is up
  if(!playerTurn && currTime >= timeUntil)
  {   
    if(!done)
    {
      if(foe->state == DEAD)
      {
        msg = "WIN";
        done = true;
        
        // Only level up on enemies up to 5 levels weaker
        if(foe->level + 5 > team->level)levelUp(team);
      }
      // Enemy Attack
      else
      {
        setAllLEDs(false);
        pending = true;
        msg = "FOE ATK";
      }
    }
    playerTurn = true;
    timeUntil = currTime + animTime;
  }
  
  // Diagonal background scroll
  static float offset = 0.0;
  
  offset -= 0.5;
  if(offset < -16.0) offset+= 16.0;
  
  animUpdate(bg, t);
  drawTileTexture(bg, offset, offset, 6);

  // Draw monsters
  animUpdate(foe->a, t);
  animUpdate(team->a, t);
  drawAnim(foe->a, 0, 0);
  drawAnim(team->a, 96, 0);

  // Draw Monster Info
  sprintf(TEXT_BUFF, "%d", foe->level);
  drawTextAt(TEXT_BUFF, 8, 24);
  sprintf(TEXT_BUFF, "%d/%d", foe->health, (int)foe->maxHealth);
  drawTextAt(TEXT_BUFF, 32, 0);
  
  sprintf(TEXT_BUFF, "%d", team->level);
  drawTextAt(TEXT_BUFF, 104, 24);
  sprintf(TEXT_BUFF, "%d/%d",team->health,(int)team->maxHealth);
  drawTextAt(TEXT_BUFF, 40, 24);

  // Draw Battle Dialog
  drawTextAt(msg, 36, 12);
}

bool isBattleDone()
{
  return done;
}

bool isSwitching()
{
  return switching;
}

bool waitingForInput()
{
  if(playerTurn && currTime >= timeUntil && !pending)
  {
    // If the leading member is dead force switch if possible
    if(team->state == DEAD && !(done || switching) )
    {
      msg = "DEAD";
      if(nextAlive == -1) done = true;
      else startSwitching();
      
      return false;
    }
    return true;
  }
  return false;
}

// This is where the actions made are actually executed after delay
void executeAction()
{
  // Delay for next action
  pending = false;
  timeUntil = currTime + animTime;
  
  if(!playerTurn)
  {
    switch(action)
    {
    case ATTACK:
      {
        int r  = monsterAttack(team, foe);
        // Set Light and message based on res
        if(r < 0) 
        {
          setLED(L_BLUE, true);
          msg = "MISS";
        }
        else if(r > 0) 
        {
          setLED(L_RED, true);
          msg = "CRIT!";
        }
        else 
        {
          setLED(L_GREEN, true);
          msg = "ATK HIT!";
        }
        break;
      }
    case CATCH:
      {
        useCapsule();
        // Higher chance if higher level and when foe's health percent is low
        if((rand() % 100) < (10 + (team->level - foe->level) + (int)(((int)foe->maxHealth - foe->health) * 100.0 / foe->maxHealth)))
        {
          setLED(L_GREEN, true);
          setLED(L_RED, true);
          caught = true;
          addMonster(foe);
          done = true;
          msg = "CAUGHT";
        }
        else msg = "MISSED";
      }
      break;
    case RUN:
      {
        // Higher chance with higher speed
        if(rand() % 100 < (50 + team->speed - foe->speed))
        {
          msg = "RAN";
          done = true;
        }
        else msg = "TRAPPED";
      }
      break;
    }
  }
  // Enemy action is only attack
  else
  {
    int r  = monsterAttack(foe, team);
    // Set Light and message based on res
    if(r < 0) 
    {
      setLED(L_BLUE, true);
      msg = "MISS";
    }
    else if(r > 0) 
    {
      setLED(L_RED, true);
      msg = "CRIT!";
    }
    else 
    {
      setLED(L_GREEN, true);
      msg = "ATK HIT!";
    }
    
  }
}

void processAction(uint8_t a)
{
  // Execute action after delay
  timeUntil = currTime + animTime;
  action = a;
  
  pending = true;
  playerTurn = false;

  switch(a)
  {
  case CATCH:
    {
      msg = "THROW!";
      break;
    }
  case RUN:
    {
      msg = "...";
      break;
    }
    // Switching doesn't use up the turn
  case SWITCH:
    {
      team = getActiveMonster();
      msg = getName(team);
      switching = false;
      playerTurn = true;
      pending = false;
      break;
    }
  }
}

void startSwitching()
{
  switching = true;
  // Save starting monster to compare to
  switchIndex = getPartyIndex();
}

void stopSwitching(int confirm)
{
  // If cancelled return to previous monster
  if(!confirm) setActiveMonster(switchIndex);

  switching = 0;

  // If new monster switch
  if(getPartyIndex() != switchIndex)
  {
    processAction(SWITCH);
  }
  else
  {
    team = getActiveMonster();
    
    // If not all members dead force switch
    if(team->state == DEAD)
    {
      if(nextAlive() == -1) done = 1;
      else switching = 1;
    }
  }
}

void setBattleMsg(const char* m)
{
  msg = m;
}

void setBattleMsgDelay(const char* m, uint64_t t)
{
  msg = m;
  timeUntil = currTime + t;
}

void exitBattle()
{
  // Free memory if not caught
  if(!caught) freeMonster(foe);
  
  foe = team = NULL;
  
  animFree(bg);
  bg = NULL;
}

