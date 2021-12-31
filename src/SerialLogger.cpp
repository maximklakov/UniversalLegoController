#include "Arduino.h"
#include "SerialLogger.h"


SerialLogger::SerialLogger(LogLevel logLevel){
    Serial.begin(9600);
    level = logLevel;
  };

void SerialLogger::debug(String string){
    if (level <= Debug) {
      Serial.print("Debug:");
      Serial.println(string);
    }
  };

void SerialLogger::error(String string){
    if (level <= Error) {
      Serial.print("Error:");
      Serial.println(string);
    }
  };

void SerialLogger::info(String string){
    if (level <= Information) {
      Serial.print("Info:");
      Serial.println(string);
    }
  };