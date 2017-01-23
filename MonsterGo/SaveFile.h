#ifndef SAVE_FILE_H
#define SAVE_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Monster.h"
#include <stdbool.h>

#pragma pack(push, 1);
  // Padding makes the struct size 108 which is 4 byte alligned
  // this allows DataFile to correctly be interpreted as a uint32_t*
  // which is what EEPROM wants
  struct DataFile
  {
    uint32_t s;
    uint8_t index;
    uint8_t capsules;
    struct MonsterSave monsterSave[5];
    uint16_t pad; // 2 byte padding
  };
#pragma pack(pop);

  extern const uint8_t MAX_SAVES;

  void initSaves();
  bool saveFile(struct DataFile* file, uint8_t slot);
  bool checkFile(uint8_t slot);
  bool deleteFile(uint8_t slot);
  struct DataFile* loadFile(uint8_t slot);

#ifdef __cplusplus
}
#endif

#endif
