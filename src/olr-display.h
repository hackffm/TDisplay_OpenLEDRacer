#ifndef OLR_DISPLAY_H
#define OLR_DISPLAY_H

#include <Arduino.h> 
#include <elapsedMillis.h>

class OlrDisp {
  
  public:
    OlrDisp();
    void begin();

    // Stuff for display
    void renderTrack();
    void renderMeters(float a, float b, float c, float d);
    void updateDisplay(int part=0);

    // Stuff for Track
    void showLampPos();

    // Stuff for DMX
    void dmxDimDown();
    void renderWinner(int widx);
    void renderCountdown(int countdown_phase);
    void renderRacing();
    void updateDMX(uint16_t startChannel=1, uint16_t endChannel=0);
    static const int maxDmxChannel = 100;
    uint8_t dmxBuf[maxDmxChannel+1];

  private:
    uint8_t msgBuf[255];
    elapsedMillis racingTime; 
    

};


extern OlrDisp Disp;

#endif