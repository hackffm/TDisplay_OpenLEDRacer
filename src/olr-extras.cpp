#include "olr-extras.h"

#include "open-led-race.h"

#include "FastLED_NeoPixel.h"
#include <FastLED.h>
#include "olr-param.h"
#include "olr-lib.h"

extern FastLED_NeoPixel<MAXLED, PIN_LED, NEO_GRB + NEO_KHZ800> track; 

void cpling::draw() {
  for(int i=0; i<4; i++) {
    track.setPixelColor( this->pos[i], COLOR_PLINGS ); 
  }
}

void cpling::doPlings() {
  static const char plingTones[4][4] = {
    {'q','e','t','i'}, // am
    {'q','r','z','i'}, // dm
    {'w','r','u','o'}, // G
    {'e','t','u','p'}, // C
  };
  for(int caridx=0; caridx<4; caridx++) {
    int posi = (int)cars[caridx].dist % tck.cfg.track.nled_main; 
    if(cars[caridx].trackID != 1) posi = 0;
    for(int i = 0; i<4; i++) {
      uint32_t bitmask = 1<<(caridx*4 + i);
      int dpos = abs(pos[i] - posi);

      // check if in or out already
      if(bitmask & this->inout) {
        // already in, wait for out
        if(dpos > 4) this->inout &= ~bitmask;
      } else {
        if(dpos < 2) {
          // now new in
          this->inout |= bitmask;
          char msg[16];
          sprintf(msg, "DongDong:%c", plingTones[caridx][i]);
          espnowSendString(msg);
        }  
      }  
    }
  }
}

cpling pling(120,140,160,180);