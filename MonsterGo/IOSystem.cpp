#include "IOSystem.h"
#include "WireUtil.h"
#include "Graphics.h"

// Using a cpp file otherwise pin codes cannot be used in const arrays and WireUtil cannot be used

// Input
extern "C"{
  static const uint32_t Switches[] = { PA_6, PA_7 };
  static const uint32_t PullupButtons[] = {PF_4, PF_0};
  static const uint32_t PushButtons[] = {PE_0, PD_2};
  static const uint32_t Buttons[] = { PullupButtons[0], PullupButtons[1], PushButtons[0], PushButtons[1]};
  static const uint32_t Potentiometer = PE_3;
  static const uint8_t Accelerometer = 0x1D;

  const size_t SWITCH_COUNT = sizeof(Switches)/sizeof(Switches[0]);
  const size_t BUTTON_COUNT = sizeof(Buttons)/sizeof(Buttons[0]);
  static const size_t PULLUP_COUNT = sizeof(PullupButtons)/sizeof(PullupButtons[0]);
  static const size_t PUSH_COUNT = sizeof(PushButtons)/sizeof(PushButtons[0]);

  static const float POT_MAX = 4096.0; // Resolution of 12 bits unsigned

  static const float MAX_ACCEL = 4 * G;
  static const float MAX_READING = 512.0; // Resolution of 10 bits signed
  static const float SHAKE_THRESHOLD = 2 * G;

  // Output
  static const uint32_t LEDs[] = {PF_1, PF_2, PF_3, PB_5, PD_6, PC_7, PC_6};
  const size_t LED_COUNT = sizeof(LEDs)/sizeof(LEDs[0]);

  const size_t LEFT = 0;
  const size_t RIGHT = 1;
  const size_t UP = 2;
  const size_t DOWN = 3;

  const size_t L_RED = 0;
  const size_t L_GREEN = 2;
  const size_t L_BLUE = 1;
  const size_t L_1 = 3;
  const size_t L_2 = 4;
  const size_t L_3 = 5;
  const size_t L_4 = 6;

  const float G = 9.81;

  struct ButtonState
  {
    bool state;
    bool prevState;
  };

  struct AccelState
  {
    float x, y, z;
    float shake;
  };

  struct IOState
  {
    AccelState accel;
    float potentiometer;
    ButtonState buttons[BUTTON_COUNT];
    bool switches[SWITCH_COUNT];
    bool leds[LED_COUNT];
  };

  static IOState io;

  void ioInit()
  {
    io = {0};
    for(size_t i = 0; i < SWITCH_COUNT; i++)
    pinMode(Switches[i], INPUT);

    // Original Buttons are Input Pullups
    for(size_t i = 0; i < PULLUP_COUNT; i++)
    pinMode(PullupButtons[i], INPUT_PULLUP);

    // Booster Buttons are regular Inputs
    for(size_t i = 0; i < PUSH_COUNT; i++)
    pinMode(PushButtons[i], INPUT);

    for(size_t i = 0; i < LED_COUNT; i++)
    {
      pinMode(LEDs[i], OUTPUT);
    }
    setAllLEDs(false);

    // I2C accelerometer initialization
    // Data format 4g, frequency 4Hz
    wireWriteRegister(Accelerometer, 0x31, 1);
    wireWriteRegister(Accelerometer, 0x2D, 1 << 3);
  }

  void inputUpdate()
  {
    for(size_t i = 0; i < SWITCH_COUNT; i++)
    io.switches[i] = digitalRead(Switches[i]);

    // Pullup and Push Buttons have inverted inputs so process them seperately
    for(size_t i = 0; i < BUTTON_COUNT; i++)
    {
      io.buttons[i].prevState = io.buttons[i].state;
      
      io.buttons[i].state = (i < PULLUP_COUNT) ? (!digitalRead(Buttons[i])) : (digitalRead(Buttons[i]));
    }

    io.potentiometer = analogRead(Potentiometer)/POT_MAX;

    // Get accelerometer state
    size_t const dataLength= 6; // 2 bytes per coordinate x,y,z
    uint32_t data[dataLength] = {0};

    wireWriteByte(Accelerometer, 0x32); // Start of data
    wireRequestArray(Accelerometer, data, dataLength);

    uint16_t xi = (data[1] << 8) | data[0];
    uint16_t yi = (data[3] << 8) | data[2];
    uint16_t zi = (data[5] << 8) | data[4];
    io.accel.x = *(int16_t*)(&xi) / MAX_READING * MAX_ACCEL;
    io.accel.y = *(int16_t*)(&yi) / MAX_READING * MAX_ACCEL;
    io.accel.z = *(int16_t*)(&zi) / MAX_READING * MAX_ACCEL;

    // Shake = magnitude of acceleration
    io.accel.shake = sqrt(io.accel.x*io.accel.x + io.accel.y*io.accel.y + io.accel.z*io.accel.z);
  }

  // Output state of input to OLED Screen
  // Mainly for debugging
  void inputDebugDisplay()
  {
    graphicsBegin();

    // Output Switch States
    drawTextAt("S:", 0, 0);
    for(size_t i = 0; i < SWITCH_COUNT; i++)
    {
      if(io.switches[i]) drawText("+");
      else drawText("-");
    }

    // Output Button States
    drawText(" B:");
    for(size_t i = 0; i < BUTTON_COUNT; i++)
    {
      if(io.buttons[i].state) drawText("+");
      else drawText("-");
    }
        
    // Output Accelerometer Readings
    sprintf(TEXT_BUFF, "%f %f %f", io.accel.x, io.accel.y, io.accel.z);
    drawTextAt(TEXT_BUFF, 0, 8);
    
    OrbitOledMoveTo(0, 16);
    drawTextAt((io.accel.shake > 16.0)? "true" : "false", 0, 16);
    
    // Output Potentiometer State
    sprintf(TEXT_BUFF, " %f", io.potentiometer);
    drawText(TEXT_BUFF);

    graphicsEnd();
  }

  bool wasButtonPressed(size_t i)
  {
    if(i >= BUTTON_COUNT) return false;
    return io.buttons[i].state && !io.buttons[i].prevState;
  }

  bool wasButtonReleased(size_t i)
  {
    if(i >= BUTTON_COUNT) return false;
    return !io.buttons[i].state && io.buttons[i].prevState;
  }

  bool wasAnyButtonPressed()
  {
    for(size_t i = 0; i < BUTTON_COUNT; i++)
    {
      if(io.buttons[i].state && !io.buttons[i].prevState) return true;
    }
    return false;
  }
  
  bool wasAnyButtonReleased()
  {
    for(size_t i = 0; i < BUTTON_COUNT; i++)
    {
      if(!io.buttons[i].state && io.buttons[i].prevState) return true;
    }
    return false;
  }

  bool isButtonPressed(size_t i)
  {
    if(i >= BUTTON_COUNT) return false;
    return io.buttons[i].state;
  }

  bool isSwitchFlipped(size_t i)
  {
    if(i >= SWITCH_COUNT) return false;
    return io.switches[i];
  }

  float getPotentiometer()
  {
    return io.potentiometer;
  }

  bool isShaking()
  {
    return io.accel.shake > SHAKE_THRESHOLD;  
  }
  float accelX()
  {
    return io.accel.x;
  }
  float accelY()
  {
    return io.accel.y;
  }
  float accelZ()
  {
    return io.accel.z;
  }

  void setLED(size_t i, bool on)
  {
    if(i >= LED_COUNT) return;
    digitalWrite(LEDs[i], on);
    io.leds[i] = on;
  }

  void toggleLED(size_t i)
  {
    if(i >= LED_COUNT) return;
    setLED(i, !io.leds[i]);
  }

  void setAllLEDs(bool on)
  {
    for(size_t i = 0; i < LED_COUNT; i++)
    setLED(i, on);
  }
}

