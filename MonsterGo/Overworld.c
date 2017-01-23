#include "Overworld.h"
#include "Resources.h"
#include "Energia.h"
#include "Player.h"
#include <time.h>
#include <stdlib.h>

const float FOUND = 50;
const float CLOSE = 100;
const float FAR = 250;

static struct Animation* worlds[2][4];
static uint8_t spawnIndex[2][3] = {{0, 1, 2}, {0, 3, 4}};
static int8_t currWorld, worldCount;
static float pos, enemyLeft, enemyRight;

// Positions for scenery graphics
static float objPos[6];

static void generateEnemyLeft()
{
  enemyLeft = pos - (rand() % (int)(FAR * 2) + CLOSE);
}

static void generateEnemyRight()
{
  enemyRight = pos + (rand() % (int)(FAR * 2) + CLOSE);
}
static void randomizeValues()
{
  generateEnemyLeft();
  generateEnemyRight();

  for(int i = 0; i < 6; i++)
  {
    objPos[i] = (rand() % 60) + ((i%2 == 0)?0 : 68); 
  }
}
static void genLeft(float *l, float *r)
{
  *r = *l;
  *l = -120 + rand() % 100;
}

static int genRight(float *l, float *r)
{
  *l = *r;
  *r = 148 + rand() % 100;
}

void drawWorld(uint64_t t)
{
  // Update all graphics
  for(int i = 0; i < 4; i++)
  {
    animUpdate(worlds[currWorld][i], t);
  }
  animUpdate(getPlayer(), t);
  

  // Sky Objects
  for(int i = 0; i < 2; i++)
  {
    objPos[i]+= 0.2;
    drawAnim(worlds[currWorld][1], (int)(objPos[i]), 8);
  }
  if(objPos[1] > 148) genLeft(&objPos[0], &objPos[1]);
  
  // Background Objects
  drawAnim(worlds[currWorld][2], (int)(objPos[2]), 16);
  drawAnim(worlds[currWorld][2], (int)(objPos[3]), 16);

  // Ground Texture
  drawTileTexture(worlds[currWorld][0], (int)pos, 22, 1);
  drawAnim(getPlayer(), 50, 0);
  
  // Foreground Objects
  drawAnim(worlds[currWorld][3], (int)(objPos[4]), 16);
  drawAnim(worlds[currWorld][3], (int)(objPos[5]), 16);
}

uint8_t getMonsterIndex()
{
  return spawnIndex[currWorld][rand() % 3];
}

void initOverworld()
{
  srand(millis());
  worldCount = 2;
  currWorld = 0;
  worlds[0][0] = addFrame(animCreate(8, 8, 1000, grass1[0], grass1[1]), grass2[0], grass2[1]);
  worlds[0][1] = addFrame(animCreate(16, 8, 1000, bird1[0], bird1[1]), bird2[0], bird2[1]);
  worlds[0][2] = animCreate(16, 8, 1000, shrub[0], shrub[1]);
  worlds[0][3] = addFrame(animCreate(16, 16, 1000, bush1[0], bush1[1]), bush2[0], bush2[1]);
  worlds[1][0] = addFrame(animCreate(8, 8, 1000, desert1[0], desert1[1]), desert2[0], desert2[1]);
  worlds[1][1] = addFrame(animCreate(16, 8, 1000, cloud1[0], cloud1[1]), cloud2[0], cloud2[1]);
  worlds[1][2] = animCreate(16, 8, 1000, pyramid[0], pyramid[1]);
  worlds[1][3] = addFrame(animCreate(16, 16, 1000, cactus1[0], cactus1[1]), cactus2[0], cactus2[1]);
  pos = 0;
  randomizeValues();
}

void movePos(float delta)
{
  static float accumulate = 0.0;
  accumulate += fabs(delta);
  
  // Decrease secondary stats for monsters
  if(accumulate >= 100)
  {
    accumulate -= 100;
    for(int i = 0; i < getPartySize(); i++)
    {
      struct Monster *m = getMonster(i);
      if(m->friendship) m->friendship--;
      if(m->tired < 100) m->tired++;
      if(m->hunger < 100) m->hunger++;

      updateState(m);
    }
  }
  
  // Gain a capsule after moving large distance
  static float move = 0.0;
  pos += delta;
  move += fabs(delta);
  if(move > 500)
  {
    move -= 500.0;
    gainCapsule();
  }
  
  // Update objects
  for(int i = 0; i < 6; i++)
  {
    objPos[i] -= delta * ((i < 2) ? 0.5 : 1);
  }

  // Generate next obj position after it leaves the scene
  if(delta > 0)
  {
    for(int i = 0; i < 6; i+=2)
    {
      if(objPos[i] <  -20.0)
      {
        genRight(&objPos[i], &objPos[i+1]);
      }
    }
  }
  else if (delta < 0)
  {
    for(int i = 1; i < 6; i+=2)
    {
      if(objPos[i] > 148.0)
      {
        genLeft(&objPos[i-1], &objPos[i]);
      }
    }
  }
  
  // Generate new enemies when passed
  if(pos > enemyRight)
  {
    enemyLeft = enemyRight;
    generateEnemyRight();
  }
  else if (pos < enemyLeft)
  {
    enemyRight = enemyLeft;
    generateEnemyLeft();
  }
}

void nextWorld()
{
  if(++currWorld >= worldCount) currWorld = 0;
  pos = 0;
  randomizeValues();
}

void prevWorld()
{
  if(--currWorld < 0) currWorld = worldCount - 1;
  pos = 0;
  randomizeValues();
}

float getProximity()
{
  float left = enemyLeft - pos, right = enemyRight - pos;
  return(abs(right) < abs(left))? right : left;
}
