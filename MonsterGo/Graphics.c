#include "Graphics.h"

char TEXT_BUFF[30] = "";

// Buffer to save the screen state
static char saveBuffer[512];

static void save()
{
  OrbitOledMoveTo(0, 0);
  OrbitOledGetBmp(128, 32, saveBuffer);
  OrbitOledClearBuffer();
}

static void restore()
{
  OrbitOledClearBuffer();
  OrbitOledMoveTo(0, 0);
  OrbitOledPutBmp(128, 32, saveBuffer);
}

void graphicsInit()
{
  OrbitOledInit();
  OrbitOledClear();
  OrbitOledClearBuffer();
}

void graphicsBegin()
{
  OrbitOledClearBuffer();
  OrbitOledSetDrawMode(modOledSet);
}

void graphicsEnd()
{
  OrbitOledUpdate();
}

void drawTileTexture(struct Animation* a, int offx, int offy, int rows)
{
  int w = animWidth(a);
  
  // Move X into bounds
  while (offx > 0) offx -= w;
  while (offx <= -w) offx += w;
  
  int start = offx;
  
  // Move Y into bounds
  while(offy <= -8)
  {
    rows--;
    offy += 8;
  }
  
  if (rows < 1) return;

  // Top row is cut off by top of screen
  if(offy < 0 && offy > -8)
  {
    // Capture portion of sprite shown
    save();
    char top[w];
    OrbitOledMoveTo(0, 0);
    OrbitOledPutBmp(w, animHeight(a), animFrame(a, 0));
    OrbitOledMoveTo(0, -offy);
    OrbitOledGetBmp(w, 8+offy, top);
    restore();
    
    // Left side cut off
    if(offx < 0)
    {
      OrbitOledMoveTo(0,0);
      OrbitOledPutBmp(w + offx, 8 + offy, top - offx);
      offx += w;
    }
    // Draw entire row
    while(offx < 128)
    {
      OrbitOledMoveTo(offx,0);
      OrbitOledPutBmp(w, 8 + offy, top);
      offx += w; 
    }
    rows--;
    offy += 8;
  }
  // For each remaining row draw normal sprite
  while(rows && offy < 32)
  {
    offx = start;
    if(offx < 0)
    {
      OrbitOledMoveTo(0,offy);
      OrbitOledPutBmp(w + offx, 8, animFrame(a,0) - offx);
      offx += w;
    }
    while(offx < 128)
    {
      OrbitOledMoveTo(offx, offy);
      OrbitOledPutBmp(w, 8, animFrame(a, 0));
      offx += w; 
    }
    offy += 8;
    rows--;
  }
}


void drawAnim(struct Animation* a, int x, int y)
{
  uint8_t w = animWidth(a), h = animHeight(a);
  if(x <= -w || h <= -h) return;
  
  char *mask = animFrame(a, 1), *bmp = animFrame(a, 0);
  char cut[((h + 7) / 8) * w];
  char cut2[((h + 7) / 8) * w];
  int offx = 0, offy = 0;
  
  // If sprite is cut off capture shown portion
  if(x < 0 || y < 0)
  {
    save();

    offx = (x < 0)? -x: 0;
    offy = (y < 0)? -y: 0;

    OrbitOledMoveTo(0, 0);
    OrbitOledPutBmp(w, h, bmp);
    OrbitOledMoveTo(offx, offy);
    OrbitOledGetBmp(w - offx, h - offy, cut);

    OrbitOledClearBuffer();
    
    OrbitOledMoveTo(0, 0);
    OrbitOledPutBmp(w, h, mask);
    OrbitOledMoveTo(offx, offy);
    OrbitOledGetBmp(w - offx, h - offy, cut2);
    restore();
    
    bmp = cut;
    mask = cut2;
  }
  
  // Draw mask using AND to set a transparency mask
  // Note: AND mode only works by 8 pixel high rows
  // So each sprite should be multiple of 8 high and
  // drawn on offset of 8
  OrbitOledSetDrawMode(modOledAnd);
  OrbitOledMoveTo(x, y);
  OrbitOledPutBmp(w - offx, h - offy, mask);
  
  // Draw the Original frame on or
  OrbitOledSetDrawMode(modOledOr);
  OrbitOledMoveTo(x, y);
  OrbitOledPutBmp(w - offx, h - offy, bmp);

  // Use original Draw Mode
  OrbitOledSetDrawMode(modOledSet);
}

void drawTextAt(const char * text, int x, int y)
{
  OrbitOledSetDrawMode(modOledSet);
  OrbitOledMoveTo(x, y);
  OrbitOledDrawString(text);
}

void drawText(const char* text)
{
  OrbitOledSetDrawMode(modOledSet);
  OrbitOledDrawString(text);
}
