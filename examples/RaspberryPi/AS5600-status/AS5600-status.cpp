#include <iostream>
#include <errno.h>
#include "AS5600.h"
#include <unistd.h>
#include <bitset>

using namespace std;

AS5600 as;

typedef bitset<8> bs8;
typedef bitset<16> bs16;

int main()
{

  uint8_t s;
  for(;;) {
    s = as.readStatus();

    cout
      << "Status: b" << bs8(s)
      << ", dir: " << (unsigned int)as.getDirection()
      << ", ZMCO: " << bs8(as.getZMCO())
      << ", Z pos: " << as.getZPosition()
      << ", M pos: " << as.getMPosition()
      << ", max angle: " << as.getMaxAngle()
      << ", CONF: " << bs16(as.getConfigure())
      << ", power mode: " << (unsigned int)as.getPowerMode()
      << ", hysterisis: " << (unsigned int)as.getHysteresis()
      << ", output mode: " << (unsigned int)as.getOutputMode()
      << ", getSlowFilter: " << (unsigned int)as.getSlowFilter()
      << ", getFastFilter: " << (unsigned int)as.getFastFilter()
      << ", agc:" << (unsigned int)as.readAGC()
      << ", magnitude: " << as.readMagnitude()
      << endl;
    usleep(100000);
  }
  return 0;
}
