#include "olr-display.h"

#include "open-led-race.h"

#include "FastLED_NeoPixel.h"
#include <FastLED.h>
#include "olr-param.h"
#include "olr-lib.h"

/** DMX setup
 * A) 001 - 036: ADJ UB 12H: RGBWAU (6 x 6Ch)
 * B) 040 - 063: 8x RGB (40,43,46,49,52,55,58,61)
 * 
 * A) For Ambi-Light,Start-Light, Race-Positions, Winning-Light
 * B) Ambi-Light for Track
 * 
 */
#define DMX_RGB_OFFSET 40

extern FastLED_NeoPixel<MAXLED, PIN_LED, NEO_GRB + NEO_KHZ800> track; 

int lampPos[8] = {20,79,115,135, 430,457,493,508};

uint8_t carColorsRGBW[][4] = {  //rgbw
  {25,0,0,0}, {0,25,0,0}, {0,0,25,0}, {0,0,0,25}
};

uint8_t carColorsRGB[][4] = {  //rgb
  {255,0,0}, {0,255,0}, {0,0,255}, {255,255,255}
};

#ifdef ENABLE_TDISPLAY
  #include "examples/Smooth Fonts/FLASH_Array/Smooth_font_reading_TFT/NotoSansBold15.h"
  #include "examples/Smooth Fonts/FLASH_Array/Smooth_font_reading_TFT/NotoSansBold36.h"

  // Do not include "" around the array name!
  #define AA_FONT_SMALL NotoSansBold15
  #define AA_FONT_LARGE NotoSansBold36
  #include <TFT_eSPI.h>
  TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

  TFT_eSprite tftbuf = TFT_eSprite(&tft); 

  #define TFT_W 135
  #define TFT_H 240

  // Callback function to provide the pixel color at x,y
  uint16_t pixelColor(uint16_t x, uint16_t y) { return tft.readPixel(x, y); }
#endif

// void espnowSendString(const char* text)

OlrDisp Disp;

static const uint8_t InversGammaTable[256] = {
  0, 21, 28, 34, 39, 43, 46, 50, 53, 56, 59, 61, 64, 66, 68, 70,
 72, 74, 76, 78, 80, 82, 84, 85, 87, 89, 90, 92, 93, 95, 96, 98,
 99,101,102,103,105,106,107,109,110,111,112,114,115,116,117,118,
119,120,122,123,124,125,126,127,128,129,130,131,132,133,134,135,
136,137,138,139,140,141,142,143,144,144,145,146,147,148,149,150,
151,151,152,153,154,155,156,156,157,158,159,160,160,161,162,163,
164,164,165,166,167,167,168,169,170,170,171,172,173,173,174,175,
175,176,177,178,178,179,180,180,181,182,182,183,184,184,185,186,
186,187,188,188,189,190,190,191,192,192,193,194,194,195,195,196,
197,197,198,199,199,200,200,201,202,202,203,203,204,205,205,206,
206,207,207,208,209,209,210,210,211,212,212,213,213,214,214,215,
215,216,217,217,218,218,219,219,220,220,221,221,222,223,223,224,
224,225,225,226,226,227,227,228,228,229,229,230,230,231,231,232,
232,233,233,234,234,235,235,236,236,237,237,238,238,239,239,240,
240,241,241,242,242,243,243,244,244,245,245,246,246,247,247,248,
248,249,249,249,250,250,251,251,252,252,253,253,254,254,255,255,
};

void OlrDisp::showLampPos() {
  for(int i=0; i<8; i++) {
    track.setPixelColor( lampPos[i], COLOR_LIGHTPOS ); 
  }
}

// Renders into DMX, phase 1-4 
void OlrDisp::renderCountdown(int countdown_phase) {
  if(countdown_phase == 5) countdown_phase = 4;
  if((countdown_phase > 4) || (countdown_phase < 1)) countdown_phase = 0;
  static const uint8_t dmxOut[5][36] = {
    {0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 
     0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00 }, // nothing
    {0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 
     0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,
     0xff,0x00,0x00,0x00,0x00,0x00, 0xff,0x00,0x00,0x00,0x00,0x00 }, // red
    {0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 
     0xff,0x00,0x00,0x00,0xff,0x00, 0xff,0x00,0x00,0x00,0xff,0x00,
     0xff,0x00,0x00,0x00,0x00,0x00, 0xff,0x00,0x00,0x00,0x00,0x00 }, // red + red
    {0xff,0xff,0x00,0x00,0x00,0x00, 0xff,0xff,0x00,0x00,0x00,0x00, 
     0xff,0x00,0x00,0x00,0xff,0x00, 0xff,0x00,0x00,0x00,0xff,0x00,
     0xff,0x00,0x00,0x00,0x00,0x00, 0xff,0x00,0x00,0x00,0x00,0x00 }, // red red yellow
    {0x00,0xff,0x00,0x00,0x00,0x00, 0x00,0xff,0x00,0x00,0x00,0x00, 
     0x00,0xff,0x00,0x00,0x00,0x00, 0x00,0xff,0x00,0x00,0x00,0x00,
     0x00,0xff,0x00,0x00,0x00,0x00, 0x00,0xff,0x00,0x00,0x00,0x00 }, // green
  };
  // memcpy(&this->dmxBuf[1], dmxOut[countdown_phase], 36);
  for(int i=0; i<36; i++) this->dmxBuf[i+1] = dmxOut[countdown_phase][i] >> 5;
  updateDMX(1,36);
  racingTime = 0;
}  

// DMX renderings
void OlrDisp::renderRacing() {
  // Renders 6 Light Traffic Light
  if(racingTime < 2500) {
    // Fade down green light
    uint8_t ch6[6] = {0,0,0,0,0,0};
    ch6[1] = 255 - constrain((racingTime * 255)/2000 ,0, 255);
    for(int i=0; i<6; i++) {
      memcpy(&this->dmxBuf[1+i*6],ch6,6);
    }
    updateDMX(1,36);
  } else {
    // Sort placings
    int place[MAX_PLAYERS]; // contains indexes
    int dist[MAX_PLAYERS];  // is cleared after sort
    int maxplace = 0;       // numbers of placings
    for(int i=0; i<MAX_PLAYERS; i++) {
      place[i] = -1;
      dist[i] = 0;
      if(cars[i].dist > 10) dist[i] = cars[i].dist;
      if(cars[i].trackID == 2) dist[i] += cars[i].dist_aux;
    }
    for(int j=0; j<MAX_PLAYERS; j++) {
      int maxdist = 0;
      for(int i=0; i<MAX_PLAYERS; i++) {
        if(dist[i] > maxdist) {
          maxdist = dist[i];
          place[j] = i;
        }
      }
      if(place[j] >= 0) {
        dist[place[j]] = 0;
        maxplace++;
      }
    }
    // DMX out
    for(int i=1; i<=36; i++) this->dmxBuf[i] = 0;
    switch(maxplace) {
      case 1:
        for(int i=0; i<6; i++) memcpy(&this->dmxBuf[i*6+1],carColorsRGBW[place[0]],4);
        break;
      case 2:
        for(int i=3; i<6; i++) memcpy(&this->dmxBuf[i*6+1],carColorsRGBW[place[0]],4);
        for(int i=0; i<3; i++) memcpy(&this->dmxBuf[i*6+1],carColorsRGBW[place[1]],4);
        break;
      case 3:
        for(int i=4; i<6; i++) memcpy(&this->dmxBuf[i*6+1],carColorsRGBW[place[0]],4);
        for(int i=2; i<4; i++) memcpy(&this->dmxBuf[i*6+1],carColorsRGBW[place[1]],4);
        for(int i=0; i<2; i++) memcpy(&this->dmxBuf[i*6+1],carColorsRGBW[place[2]],4);
        break;
      case 4:
        for(int i=4; i<6; i++) memcpy(&this->dmxBuf[i*6+1],carColorsRGBW[place[0]],4);
        for(int i=2; i<4; i++) memcpy(&this->dmxBuf[i*6+1],carColorsRGBW[place[1]],4);
        for(int i=1; i<2; i++) memcpy(&this->dmxBuf[i*6+1],carColorsRGBW[place[2]],4);
        memcpy(&this->dmxBuf[0*6+1],carColorsRGBW[place[3]],4);
        break;        
    }  
  }   
  
  // Render position lamps
  memset(&this->dmxBuf[DMX_RGB_OFFSET],0,8*3);
  for(int caridx=0; caridx<4; caridx++) {
    int posi = (int)cars[caridx].dist % tck.cfg.track.nled_main; 
    if(cars[caridx].trackID != 1) posi = 0;
    for(int i = 0; i<8; i++) {
      int dpos = abs(lampPos[i] - posi);

      if(dpos < 16) {
        this->dmxBuf[DMX_RGB_OFFSET+i*3+0] |= (carColorsRGB[caridx][0] >> (dpos/2));
        this->dmxBuf[DMX_RGB_OFFSET+i*3+1] |= (carColorsRGB[caridx][1] >> (dpos/2));
        this->dmxBuf[DMX_RGB_OFFSET+i*3+2] |= (carColorsRGB[caridx][2] >> (dpos/2));
      }
 
    }
  }

}

void OlrDisp::dmxDimDown() {
  static elapsedMillis dly = 0;
  if(dly > 100) {
    dly = 0;
    for(int i=0; i<65; i++) if(this->dmxBuf[i]>0) (this->dmxBuf[i])--;
  }
}  

void OlrDisp::renderWinner(int widx) {
  for(int i=0; i<6; i++) memcpy(&this->dmxBuf[i*6+1],carColorsRGBW[widx],4);
  for(int i=0; i<8; i++) memcpy(&this->dmxBuf[i*3+DMX_RGB_OFFSET],carColorsRGB[widx],3);
}

void OlrDisp::renderTrack() {
  uint32_t start_us = micros();
  const int sizeX = 5;
  const int sizeY = 4;
  const int spaceX = 1;
  const int spaceY = 1;
  const int countX = 130/(sizeX+spaceX);
  int tlen = track.numPixels();
  for(int y=0; y<(tlen/countX)+1; y++) {
    for(int x=0; x<countX; x++) {
      int idx = x+(y*countX);
      if(idx < tlen) {
        uint32_t c = track.getPixelColor(idx);
        uint8_t b = InversGammaTable[(c >> 0) & 0xff];
        uint8_t g = InversGammaTable[(c >> 8) & 0xff];
        uint8_t r = InversGammaTable[(c >> 16) & 0xff];
        tftbuf.fillRect(4+x*(sizeX+spaceX),36+y*(sizeY+spaceY),sizeX,sizeY,tft.color565(r,g,b));
      }
    }
  }
  //tftbuf.pushSprite(0,0);
  // 9.5ms derzeit...
  //Serial.printf(" DT us:%d\r\n", micros() - start_us);
  //Serial.printf(" 12:%06x\r\n", tft.color24to16(track.getPixelColor(12)));
}

void OlrDisp::renderMeters(float a, float b, float c, float d) {
  const int startY = 200; // 240 max
  tftbuf.fillRect(0,startY,135,240-startY,TFT_BLACK);
  tftbuf.setTextColor(TFT_GREENYELLOW);
  tftbuf.setCursor(0, startY);
  tftbuf.printf("%.3f %.3f\r\n%.3f %.3f", a,b,c,d);
}   

void OlrDisp::updateDisplay(int part) {
  if(part == 0) tftbuf.pushSprite(0,0); 
  else if(part == 1) tftbuf.pushSprite(0,0,0,0,TFT_W,TFT_H/4);
  else if(part == 2) tftbuf.pushSprite(0,TFT_H/4,0,TFT_H/4,TFT_W,TFT_H/4);
  else if(part == 3) tftbuf.pushSprite(0,TFT_H/4*2,0,TFT_H/4*2,TFT_W,TFT_H/4);
  else if(part == 4) tftbuf.pushSprite(0,TFT_H/4*3,0,TFT_H/4*3,TFT_W,TFT_H/4);
}

OlrDisp::OlrDisp() {

}

void OlrDisp::begin() {
  #ifdef ENABLE_TDISPLAY
    tft.begin();
    tft.setCallback(pixelColor);  // Switch on color callback for anti-aliased fonts
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);

    tft.setTextColor(TFT_YELLOW);
    tft.fillScreen(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
 //   tft.setTextSize(2);
    
    tftbuf.setColorDepth(16);
    tftbuf.createSprite(tft.width(), tft.height());
    tftbuf.fillSprite(TFT_BLACK);

    for(int x=0; x<tft.width(); x++) {
      for(int y=0; y<tft.height(); y++) {
        tftbuf.drawPixel(x,y,tft.color565((x*64)/tft.width(),(y*64)/tft.height(), ((tft.height()-y)*64)/tft.height()));
      }
    }  
    
    tftbuf.setTextColor(TFT_YELLOW);
    tftbuf.setCursor(0, 10);
    tftbuf.loadFont(AA_FONT_SMALL);
    tftbuf.setTextDatum(MC_DATUM);
    tftbuf.drawString("Open LED Race", tft.width() / 2, 10);

    tftbuf.pushSprite(0,0);
    // tft.drawString("Race", tft.width() / 2, 26);
   // tft.println("Open LED\nRace");
  #endif
  for(int i=0; i<=this->maxDmxChannel; i++) this->dmxBuf[i] = 0;
}

void OlrDisp::updateDMX(uint16_t startChannel, uint16_t endChannel) {
  if(endChannel == 0) endChannel = maxDmxChannel;
  if(endChannel > maxDmxChannel) endChannel = maxDmxChannel;
  startChannel = constrain(startChannel, 1, endChannel);
  int numChannels = endChannel - startChannel + 1;
  if(numChannels > 0) {
    char cbuf[4];
    static const char hexbuf[] = "0123456789abcdef";
    sprintf((char *)this->msgBuf, "DMXA:%03d", (int)startChannel);
    cbuf[2] = 0;
    uint8_t *pDmxBuf = &(this->dmxBuf[startChannel]);
    for(int i=startChannel; i<=endChannel; i++) {
      cbuf[0] = hexbuf[*pDmxBuf >> 4];
      cbuf[1] = hexbuf[*pDmxBuf++ & 0x0f];
      strcat((char *)this->msgBuf, cbuf);
    }
    espnowSendString((char *)this->msgBuf);
    //Serial.println((char *)this->msgBuf);
  }
}  