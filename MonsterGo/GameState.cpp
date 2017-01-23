#include "GameState.h"
#include "IOSystem.h"
#include "Resources.h"
#include "Animation.h"
#include "Graphics.h"
#include "Overworld.h"
#include "Monster.h"
#include "Player.h"
#include "Battle.h"
#include "Train.h"
#include "SaveFile.h"

enum states{WALK, BATTLE, TRAIN, SAVE};

static uint8_t gameState = 0;
static uint8_t level = 0;

void gameInit()
{  Serial.println(sizeof(struct DataFile));
  Serial.println(sizeof(struct MonsterSave));

  initMonsterTypes();
  initSaves();
  createPlayer();
  initOverworld();
}

// Debug Output to Validate File Contents upon save/load
static void debugFile(struct DataFile* f)
{  if(!f) return;
  Serial.println(String("Size: ") + String(f->s));
  Serial.println(String("Index: ") + String(f->index));
  Serial.println(String("Capsules: ") + String(f->capsules));
  for(int i = 0; i < f->s; i++)
  {
    Serial.println(String("\nMonsterSave") + String(i));
    Serial.println(String("MaxHP: ") + String(f->monsterSave[i].maxHealth));
    Serial.println(String("Power: ") + String(f->monsterSave[i].power));
    Serial.println(String("Speed: ") + String(f->monsterSave[i].speed));
    Serial.println(String("HP: ") + String(f->monsterSave[i].health));
    Serial.println(String("LVL: ") + String(f->monsterSave[i].level));
    Serial.println(String("STATE: ") + String(f->monsterSave[i].state));
    Serial.println(String("FRIEND: ") + String(f->monsterSave[i].friendship));
    Serial.println(String("TIRED: ") + String(f->monsterSave[i].tired));
    Serial.println(String("HUNGER: ") + String(f->monsterSave[i].hunger));
    Serial.println(String("INDEX: ") + String(f->monsterSave[i].index));
  }
  Serial.println("\n");
}

static void saveState(uint64_t t)
{  
  static int save = 0;
  static int choice = 0;
  static char* CHOICES[] = {"NEW", "LOAD", "SAVE", "DELETE"};
  static const size_t max_choice = sizeof(CHOICES)/sizeof(CHOICES[0]);

  static int menu = 0;
  static char* msg = "R";

  setAllLEDs(false);
  
  // Return to overworld
  if(!isSwitchFlipped(LEFT))
  {
    menu = 0;
    gameState = WALK;
    msg = "R";
    prevWorld();
    nextWorld();
    return;
  }

  // Update after input
  int update = wasAnyButtonPressed();
  
  // Display the screen upon entering
  if(msg == "R") {
    msg = "";
    update = 1;
  }
  
  // Navigate Options
  if(wasButtonPressed(DOWN))
  {
    if(!menu)
    {
      if(++choice >= max_choice) choice = 0;
    }
    else
    {
      if(++save >= MAX_SAVES) save = 0;
    }
    msg = "";
  }
  if(wasButtonPressed(UP)) 
  {
    if(!menu)
    {
      if(--choice < 0) choice = max_choice - 1;
    }
    else
    {
      if(--save < 0) save = MAX_SAVES -1;
    }
    msg = "";
  }
  // Back
  if(wasButtonPressed(LEFT))
  {
    menu = 0;
    msg = "";
  }
  // Enter
  if(wasButtonPressed(RIGHT))
  {
    if(!menu)
    {
      if(choice == 0) 
      {
        createPlayer();
        msg = "New Game Created";
      }
      else menu = 1;
    }
    else
    {
      switch(choice)
      {
      case 1: // LOAD
        {
          if(checkFile(save))
          {
            struct DataFile* f = loadFile(save);
            debugFile(f);
            loadPlayer(f);
            free(f);
            msg = "Game Loaded";
          }
          else 
          {
            msg = "No Save Found";
          }
          break;
        }
      case 2: // SAVE
        {
          struct DataFile* f = savePlayer();        
          
          if(saveFile(f, save))
          msg = "File Saved";
          else
          msg = "Save Failed";
          debugFile(f);
          free(f);
          break;
        }
      case 3: // DELETE
        {
          if(checkFile(save))
          {
            deleteFile(save);
            msg = "File Deleted";
          }
          else
          {
            msg = "No Save Found";
          }
          break;
        }
      }

    }
  }
  
  // Update Monitor
  if(update)
  {
    graphicsBegin();
    drawTextAt(CHOICES[choice], 0, 0);
    if(menu)
    {
      char n[3];
      sprintf(n, "%d", save + 1);
      drawTextAt(n, 64, 0);
    }
    drawTextAt(msg, 0, 16);
    graphicsEnd();
  }
}

static void walkState(uint64_t t)
{  
  if(isSwitchFlipped(LEFT))
  {
    gameState = SAVE;
    return;
  }
  if(isSwitchFlipped(RIGHT))
  {
    gameState = TRAIN;
    createTrain(t);
    return;
  }
  
  // Tilt Left
  if(accelX() > G * 0.5) movePos(-0.1);
  // Tilt Right
  if(accelX() < -G* 0.5) movePos(0.1);
  
  // Change Worlds
  if(wasButtonPressed(UP)) prevWorld();
  if(wasButtonPressed(DOWN)) nextWorld();

  // Set Difficulty
  level = getPotentiometer() * 99 + 1.5;
  
  const size_t led[4] = {L_1, L_2, L_3, L_4};
  setAllLEDs(false);
  float near = getProximity();

  uint8_t on = 1;
  if(abs(near) < FAR) on++;
  if(abs(near) < CLOSE) on++;
  if(abs(near) < FOUND) on++;
  
  for(int i = 0; i < on; i++)
  {
    if(near < 0) setLED(led[i], true);
    else setLED(led[3-i], true);
  }

  if(abs(near) < FOUND)
  {
    if(getActiveMonster()->state == DEAD)
    {
      if(nextAlive() != -1) setActiveMonster(nextAlive());
      else
      {
        setLED(L_RED, true);
        return;
      }
    }
    
    setLED(L_GREEN, true);
    setLED(L_BLUE, true);
    
    if(isShaking())
    {
      gameState = BATTLE;
      setAllLEDs(false);
      createBattle(monsterIndex[getMonsterIndex()], level, t);
    }
  }

}

static void battleState(uint64_t t)
{  
  static const char* CHOICES[] = {"ATK", "SWITCH", "CATCH", "RUN"};
  static const size_t max_choice = sizeof(CHOICES)/sizeof(CHOICES[0]);
  static int choice = 0;

  if(waitingForInput())
  {
    // Battle Done
    if(isBattleDone())
    {
      if(wasAnyButtonPressed())
      {
        exitBattle();
        if(getActiveMonster()->state == DEAD && nextAlive() != -1)
        {
          setActiveMonster(nextAlive());
        }
        // Reset World by swapping to and back
        gameState = WALK;
        nextWorld();
        prevWorld();
      }
    }
    // Switching Monsters
    else if(isSwitching())
    {
      if(wasButtonPressed(UP) && prevAlive() != -1) setActiveMonster(prevAlive());
      if(wasButtonPressed(DOWN) && nextAlive() != -1) setActiveMonster(nextAlive());
      if(wasButtonPressed(RIGHT)) stopSwitching(1);
      if(wasButtonPressed(LEFT)) stopSwitching(0);
    }
    // Input Command
    else
    {
      setAllLEDs(false);
      setBattleMsg(CHOICES[choice]);
      if(wasButtonPressed(UP))
      if(--choice < 0) choice = max_choice - 1;
      if(wasButtonPressed(DOWN))
      if(++choice >= max_choice) choice = 0;
      if(wasButtonPressed(RIGHT))
      {
        switch(choice)
        {
        case 0:
          processAction(ATTACK);
          break;
        case 1:
          if(nextAlive() != -1)
          {
            startSwitching();
            setBattleMsg("SELECT");
          }
          else setBattleMsgDelay("NO MON", 1000);
          break;
        case 2:
          if(getCapsules() > 0 && getPartySize() < getMaxPartySize())
          {
            processAction(CATCH);
          }
          else setBattleMsgDelay("NO CAPS", 1000);
          break;
        case 3:
          processAction(RUN);
          break;
        }
      }
    }
  }
}

static void trainState(uint64_t t)
{  
  static const char* CHOICES[] = {"PLAY", "SLEEP", "FEED", "FREE"};
  static const size_t max_choice = sizeof(CHOICES)/sizeof(CHOICES[0]);
  static int choice = 0;

  if(isSwitchFlipped(LEFT))
  {
    gameState = SAVE;
    exitTrain();
    return;
  }
  if(!isSwitchFlipped(RIGHT))
  {
    gameState = WALK;
    exitTrain();
    return;
  }

  if(selectingMonster())
  {
    setAllLEDs(false);
    if(wasButtonPressed(UP)) 
    {
      if(getPartyIndex() - 1 < 0) setActiveMonster(getPartySize()-1);
      else setActiveMonster(getPartyIndex() - 1);
    }
    if(wasButtonPressed(DOWN)) 
    {
      if(getPartyIndex() + 1 >= getPartySize()) setActiveMonster(0);
      else setActiveMonster(getPartyIndex() + 1);
    }
    if(wasButtonPressed(RIGHT))
    {
      stopSelecting();
    }
  }
  else if(trainMenu())
  {
    setAllLEDs(false);
    struct Monster* m = getActiveMonster();
    
    if(wasButtonPressed(UP))
      if(--choice < 0) choice = max_choice - 1;
    if(wasButtonPressed(DOWN))
      if(++choice >= max_choice) choice = 0;
    
    setTrainMsg(CHOICES[choice]);
    if(wasButtonPressed(RIGHT)){
      switch(choice)
      {
      case 0: 
        if(m->friendship < 100)startTraining(PLAY);
        else setTrainMsgDelay("FRIEND", 1500);
        break;
      case 1:
        if(m->tired || m->health < (int)m->maxHealth)startTraining(SLEEP);
        else setTrainMsgDelay("AWAKE", 1500);
        break;
      case 2:
        if(m->hunger)startTraining(FEED);
        else setTrainMsgDelay("FULL", 1500);
        break;
      case 3:
        if(getPartySize() > 1) startTraining(FREE);
        else setTrainMsgDelay("ONLY 1", 1500);
        break;
      }
    }
    if(wasButtonPressed(LEFT))
    {
      startSelecting();
    }
  }
}

void processFrame(uint64_t t)
{  switch(gameState)
  {
  case WALK:
    walkState(t);
    break;
  case BATTLE:
    battleState(t);
    break;
  case TRAIN:
    trainState(t);
    break;
  case SAVE:
    saveState(t);
  default:
    break;
  }
}

void drawFrame(uint64_t t)
{  
  if(gameState == SAVE) return;
  graphicsBegin();
  
  switch(gameState)
  {
  case WALK:
    {
      drawWorld(t);
      String t = String("lvl:") + String(level);
      char text[10];
      t.toCharArray(text, 10);
      drawTextAt(text, 0, 0);

      t = String("caps:") + String(getCapsules());
      t.toCharArray(text, 10);
      drawTextAt(text, 80, 0);
      break;
    }
  case BATTLE:
    {
      drawBattle(t);
      
      break;
    }
  case TRAIN:
    {
      drawTrain(t);
      break;
    }
  }

  graphicsEnd();

}
