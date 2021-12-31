#ifndef SerialLogger_h
#define SerialLogger_h

#include "Arduino.h"

enum LogLevel
{
  Trace = 0,
  Debug = 1,
  Information = 2,
  Warning = 3,
  Error = 4,
  Critical = 5	

};

class SerialLogger
{

public:
  // constructor
  SerialLogger(LogLevel logLevel = Critical);
  void debug(String string);
  void error(String string);
  void info(String string);
  
  LogLevel level;
};
#endif