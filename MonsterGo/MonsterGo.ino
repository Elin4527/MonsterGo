#include "IOSystem.h"
#include "WireUtil.h"
#include "GameState.h"
#include "Graphics.h"

#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>
#include <stdlib.h>

void setup() {
  Serial.begin(9600);
  
  wireInit();
  ioInit();
  graphicsInit();
  gameInit();
  
  delay(100);
}

void loop() {
  const uint64_t frameDelay = 1000 / 30;  // 30 FPS
  
  static uint64_t nextUpdate = frameDelay;
  uint64_t currentTime = millis();

  // Update input every frame
  inputUpdate();
  processFrame(currentTime);

  // Draw at fixed fps
  if(currentTime >= nextUpdate)
  {
    nextUpdate += frameDelay;
    drawFrame(currentTime);
    //inputDisplay();
  }
}
