#include "Train.h"
#include "Animation.h"
#include "Player.h"
#include "Graphics.h"
#include "Resources.h"
#include "IOSystem.h"

static struct Animation* bg, *pbar, *fill;
static struct Monster *team;

static uint64_t animTime = 1500;
static uint64_t timeUntil = 0;
static uint64_t currTime = 0;
static uint8_t action;

static bool pending;
static bool choosing;
static const char* msg;

void createTrain(uint64_t t)
{
  // Create graphics
  bg = animCreate(16, 8, 1000, train[0], train[1]);
  pbar = animCreate(64, 8, 1000, bar[0], bar[1]);
  fill = animCreate(1, 8, 1000, progress[0], progress[1]);
  team = getActiveMonster();

  currTime = t;
  timeUntil = t;
  pending = false;
  
  // Start by selecting monster
  startSelecting();
}

static void complete()
{
  setLED(L_GREEN, true);
  pending = false;
  timeUntil = currTime + animTime;
  setTrainMsg("DONE");
}

// Minigame to increase happiness by alternating buttons
static void play(){
  static int i = 0;
  static int count = 0;
  if(count < 10){
    if(!i)
    {
      setTrainMsg("LEFT");
      if(isButtonPressed(LEFT) && !isButtonPressed(RIGHT))
      {
        i = 1;
        count++;
      }
    }
    else
    {
      setTrainMsg("RIGHT");
      if(isButtonPressed(RIGHT) && !isButtonPressed(LEFT))
      {
        i = 0;
        count++;
      }
    } 
  }
  else
  {
    complete();
    count = 0;
    getActiveMonster()->friendship += 10;
    if(team->friendship > 100) team->friendship = 100;
  }
}

// Minigame to decrease hunger by shaking
static void feed()
{
  static int timetick=0;
  static const int maxNum=10;
  if(timetick < maxNum){
    if(isShaking()){
      timetick++;
    }
  }
  else
  {
    complete();
    timetick = 0;
    if(team->hunger < 10) team->hunger = 0;
    else team->hunger -= 10; 
  }
}

// Wait to recover in rest mode
static void rest()
{
  static int tick = 0;
  if(currTime >= tick)
  {
    tick = currTime + 300;
    
    // Decrease fatique
    if(team->tired > 0)
    {
      team->tired--;   
    }
    
    // Recover health
    if(team->health < (int)team->maxHealth)
    {
      team->health++;
    }
    if((!team->tired && team->health == (int)team->maxHealth) || isButtonPressed(LEFT))
    {
      complete();
      tick = currTime;
    }
  }
}

static void release()
{
  if(isButtonPressed(LEFT))
  {
    pending = false;
  }
  else if(isButtonPressed(RIGHT))
  {
    removeMonster(getPartyIndex());
    setActiveMonster(0);
    complete();
  }
}

void training()
{
  updateState(team);
  switch(action)
  {
    case PLAY:
    {
      play(); 
      break;
    }     
    case FEED:
    {
      feed();
      break;
    }
    break;
    case SLEEP:
    {
      rest();
      break;  
    }
    case FREE:
    {
      release();
      break;
    }
  }
}

void drawTrain(uint64_t t)
{
  currTime = t;
  team = getActiveMonster();

  if(pending && currTime >= timeUntil)
  {
    training();
  }
  
  // Diagonal Scrolling Background
  static float offset = 0.0;
  offset -= 0.5;
  if(offset < -16.0) offset+= 16.0;
  animUpdate(bg, t);
  drawTileTexture(bg, offset, offset, 6);
  
  static int vals[3];
  vals[0] = team->friendship;
  vals[1] = team->tired;
  vals[2] = team->hunger;
  
  // Draw Bars for each stat
  for(int i = 0; i < 3; i++)
  {
    drawAnim(pbar,40, i * 8);
    for(int j = 0; j < (int)(vals[i] * 64.0 / 100.0); j++)
    {
      drawAnim(fill, 40 + j, i * 8);
    }
    sprintf(TEXT_BUFF, "%d", vals[i]);
    drawTextAt(TEXT_BUFF, 104, i * 8);
  }
  
  animUpdate(team->a, t);
  drawAnim(team->a, 0, 0);
  
  sprintf(TEXT_BUFF, "%d:%d/%d", team->level, team->health, (int)team->maxHealth);
  drawTextAt(TEXT_BUFF, 0, 24);

  drawTextAt(msg, 72, 24);
}

bool trainMenu()
{
  return !pending && !choosing && (currTime > timeUntil);
}

bool selectingMonster()
{
  return choosing;
}

void startSelecting()
{
  choosing = 1;
  msg = "CHOOSE";
}

void stopSelecting()
{
  choosing = 0;
}

void startTraining(uint8_t a)
{
  timeUntil = currTime + 750;
  pending = 1;
  action = a;
  switch(a)
  {
    case FEED: msg = "SHAKE"; break;
    case SLEEP: msg = "WAIT"; break;
    case FREE: msg = "SURE?"; break;
  }
}

void setTrainMsg(const char* m)
{
  msg = m;
}

void setTrainMsgDelay(const char* m, uint64_t t)
{
  msg = m;
  timeUntil = currTime + t;
}

void exitTrain()
{  
  animFree(bg);
  animFree(pbar);
  animFree(fill);
  bg = pbar = fill = team = NULL;
}
