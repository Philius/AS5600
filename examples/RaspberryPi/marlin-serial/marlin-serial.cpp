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
  ss << '\n';
  string s = ss.str();
  cout << "Sending \"" << s << '"' << flush;
  serialPuts(printerFd, s.c_str());
  //serialFlush(printerFd);
  usleep(100000);
  int n;
  for(n = 0; n < 10; ++n) {
  int c = serialGetchar(printerFd);
  if(c == 'O') {
    usleep(1000);
    c = serialGetchar(printerFd);
    if(c == 'K') {
      do {
        usleep(1000);
        if(serialDataAvail(printerFd))
          c = serialGetchar(printerFd);
        usleep(1000);
      } while(serialDataAvail(printerFd));
    }else if(c == -1) {
      cout << "Timed out waiting for a response from the printer(K)." << endl;
      return false;
    }else{
      cout << "Got unexpected response from printer [1](" << (char)c << ")." << endl;
      return false;
    }
  }else if(c == -1) {
    cout << "Timed out waiting for a response from the printer(O)." << endl;
    usleep(100000);
    continue;
  }else{
    cout << "Got unexpected response from printer[0](" << (char)c << ")." << endl;
    return false;
  }
  }
  if(n > 9)
    return false;
  return true;
}

int main()
{
  uint8_t status = as.readStatus(), agc = as.readAGC();
  // Status: b01100111, agc:132
  cout << "Status: b" << bs8(status)
    << ", agc:" << (unsigned int)agc
    << endl;
  if(status & 8) {
    cerr << "Magnat too strong!" << endl;
    return 1;
  }
  if(status & 16) {
    cerr << "Magnat too weak!" << endl;
    return 1;
  }
  if(!(status & 32)) {
    cerr << "Magnat not detected!" << endl;
    return 1;
  }
  if((agc > 64) && (agc < 200))
    cout << "Magnat AGC looks good." << endl;
  else
    cout << "Magnat AGC doesn't look great." << endl;

  cout << "Connecting to printer controller" << endl;
  printerFd = serialOpen("/dev/ttyUSB0", 115200);
  if(printerFd ==-1) {
    cerr << "Unable to open serial device." << endl;
    return 1;
  }
  for(;;) {
    usleep(1000);
    int c = serialDataAvail(printerFd);
    if( c < 1) {
      continue;
    }
    else if( c == -1) {
      cerr << "serialDataAvail(printerFd) FAILED." << endl;
      break;
    }
    for(int i = 0; i < c; ++i)
      cout << (char)serialGetchar(printerFd);
  }
  return 0;
}
