#ifndef OLR_EXTRAS_H
#define OLR_EXTRAS_H

#include <Arduino.h> 
#include <elapsedMillis.h>

class cpling {
  public:
    cpling(int pos1, int pos2, int pos3, int pos4) {
      this->pos[0] = pos1; 
      this->pos[1] = pos2; 
      this->pos[2] = pos3; 
      this->pos[3] = pos4;
      this->inout = 0;
    }
    void draw();
    void doPlings();
    
  
  private:
    int pos[4];
    uint32_t inout; // bitfield


};

extern cpling pling;
 

#endif
