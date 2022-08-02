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
  cout << "Status: b" << bs8(as.readStatus())
    << ", agc:" << (unsigned int)as.readAGC()
    << endl;
  for(;;) {
    cout << "Angle: " << as.readAngle() /*
      << "(raw " << as.rawAngle() << ')'*/ << endl;
    usleep(100000);
  }
  return 0;
}
