#include <wiringSerial.h>
#include <iostream>
#include <errno.h>
#include "AS5600.h"
#include <unistd.h>
#include <bitset>
#include <sstream>
#include <cstdarg>

using namespace std;

AS5600 as;

typedef bitset<8> bs8;
typedef bitset<16> bs16;

int printerFd;

bool getResponse(const string & wanted, int count = 100000)
{
  stringstream ss;
  for(int n = 0; n < count; ++n) {
    usleep(1000);
    int c, a = serialDataAvail(printerFd);
    if( a < 1)
      continue;
    for(int i = 0; i < a; ++i) {
      c = serialGetchar(printerFd);
      cout << (char)c << flush;
      if(c == -1)
        break;
      else if(c == '\n') {
       if(ss.str() == wanted) {
         cout << "Got response \"" << wanted << "\"." << endl;
         return true;
       }
       ss.str("");
     }else
       ss << (char)c;
    }
  }
  if(wanted.length())
    cout << "Didn't get response \"" << wanted << "\"." << endl;
  return wanted.length() != 0;
}

bool sendCmd(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  stringstream ss;
  while (*fmt != '\0') {
    if (*fmt == 'd') {
      ss << va_arg(args, int);
    }else if (*fmt == 'u') {
      ss << va_arg(args, unsigned int);
    } else if (*fmt == 's') {
      // note automatic conversion to integral type
      ss << va_arg(args, const char *);
    } else if (*fmt == 'f') {
      double d = va_arg(args, double);
      ss << d;
    }
    ++fmt;
  }
  va_end(args);
  string s = ss.str();
  ss << '\n';
  cout << "Sending \"" << s << '"' << endl;
  serialPuts(printerFd, ss.str().c_str());
  //serialFlush(printerFd);
  return getResponse("ok");
}

int work()
{
  float frot = float(as.readAngle())/4096.0*360.0;
  cout << "Angle = " << frot << " degrees." << endl;
  float dist = -frot/360.0*20.0;
  cout << "Distance = " << dist << "mm." << endl;

  if((-dist) > 0) {
    cout << "Moving rotor to (or close to) absolute position zero." << endl;
    if(!sendCmd("sfs", "G0 E", dist , " F600"))
      return 1;
    frot = float(as.readAngle())/4096.0*360.0;
    cout << "NEW Angle = " << frot << " degrees." << endl;
  }else{
    cout << "No need to move rotor." << endl;
  }
/*  cout << "Homing all axes..." << endl;
  if(!sendCmd("s", "G28"))
    return 1;
  cout << "Done. Moving camera into position..." << endl;
  if(!sendCmd("s", "G0 X203 Y66 Z34.9 F600"))
    return 1;
*/
  return 0;
}

int main()
{
  uint8_t status = as.readStatus(), agc = as.readAGC();
  // Status: b01100111, agc:132
  cout << "Status: b" << bs8(status)
    << ", agc:" << (unsigned int)agc
    << endl;
  if(status & 8) {
    cerr << "Magnet too strong!" << endl;
    return 1;
  }
  if(status & 16) {
    cerr << "Magnet too weak!" << endl;
    return 1;
  }
  if(!(status & 32)) {
    cerr << "Magnet not detected!" << endl;
    return 1;
  }
  if((agc > 64) && (agc < 200))
    cout << "Magnet AGC looks good." << endl;
  else
    cout << "Magnet AGC doesn't look great." << endl;

  cout << "Connecting to printer controller" << endl;
  printerFd = serialOpen("/dev/ttyUSB0", 115200);
  if(printerFd ==-1) {
    cerr << "Unable to open serial device"<<endl;
    return 1;
  }
  getResponse("", 4000);
  work();
  cout << "All done: homing." << endl;
  if(!sendCmd("s", "G28"))
    return 1;
  cout << "Disabling steppers." << endl;
  if(!sendCmd("s", "M18")) // Disable steppers.
    return 1;
  return 0;
}
