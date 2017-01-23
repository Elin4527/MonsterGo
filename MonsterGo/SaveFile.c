#include "SaveFile.h"

#include <eeprom.h>
#include <stdlib.h>
#include <string.h>

const uint8_t MAX_SAVES = 3;
const uint32_t CHECK_CODE = 0x53450000;

void initSaves()
{
  EEPROMInit();
  uint32_t checkInit = 0;
  EEPROMRead(&checkInit, 0, 4);
  if(checkInit != CHECK_CODE) allocateSaveSpace();
}

void allocateSaveSpace()
{
  // First time program run on board, clear the EEPROM
  // and prepare it for storing save files
  EEPROMMassErase();
  
  uint32_t code = CHECK_CODE;
  EEPROMProgram(&code, 0, 4);
  
  struct DataFile f;
  memset(&f, 0, sizeof(struct DataFile));
  for(int i = 0; i < MAX_SAVES; i++)
  {
    EEPROMProgram(&f, 4 + i * sizeof(struct DataFile), sizeof(struct DataFile));
  }
}

bool saveFile(struct DataFile* file, uint8_t slot)
{
  if(slot >= MAX_SAVES || !file) return false;
  
  EEPROMProgram(file, 4 + slot*sizeof(struct DataFile), sizeof(struct DataFile));
  return true;
}

bool checkFile(uint8_t slot)
{
  if(slot >= MAX_SAVES) return false;
  
  // Check only the size member to quickly tell whether or not there is data
  // All saves will have a party
  uint32_t size;
  EEPROMRead(&size, 4 + slot * sizeof (struct DataFile), 4);
  if(size) return true;
  return false;
}

bool deleteFile(uint8_t slot)
{
  if(slot >= MAX_SAVES) return false;
  
  struct DataFile f;
  memset(&f, 0, sizeof(struct DataFile));
  EEPROMProgram(&f, 4 + slot * sizeof(struct DataFile), sizeof(struct DataFile));
  return true;
}


struct DataFile* loadFile(uint8_t slot)
{
  if(slot >= MAX_SAVES) return NULL;
  
  struct DataFile* f = (struct DataFile*) malloc(sizeof(struct DataFile));
  EEPROMRead(f, 4 + slot * sizeof(struct DataFile), sizeof(struct DataFile));
  return f;
}
