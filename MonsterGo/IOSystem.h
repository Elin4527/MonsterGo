#ifndef IO_SYSTEM_H
#define IO_SYSTEM_H

#include "Energia.h"


#ifdef __cplusplus
extern "C" {
#endif

#include <delay.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>
#include <stdbool.h>

  // Input Constants
  extern const size_t SWITCH_COUNT;
  extern const size_t BUTTON_COUNT;
  extern const size_t LEFT;
  extern const size_t RIGHT;
  extern const size_t UP;
  extern const size_t DOWN;

  // Output Constants
  extern const size_t LED_COUNT;
  extern const size_t L_RED;
  extern const size_t L_GREEN;
  extern const size_t L_BLUE;
  extern const size_t L_1;
  extern const size_t L_2;
  extern const size_t L_3;
  extern const size_t L_4;

  extern const float G;


  void ioInit();
  void inputDebugDisplay();
  void inputUpdate();

  // Getters
  bool wasAnyButtonPressed();
  bool wasAnyButtonReleased();
  bool wasButtonPressed(size_t i);
  bool wasButtonReleased(size_t i);
  bool isButtonPressed(size_t i);
  bool isSwitchFlipped(size_t i);
  float getPotentiometer();
  bool isShaking();
  float accelX();
  float accelY();
  float accelZ();

  // Setters
  void setLED(size_t l, bool on);
  void toggleLED(size_t l);
  void setAllLEDs(bool on);

#ifdef __cplusplus
}
#endif

#endif
