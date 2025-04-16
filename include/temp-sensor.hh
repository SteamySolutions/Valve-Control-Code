#pragma once

#include <Wire.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class TempSensor{
  DallasTemperature * sensor;

  public:
    TempSensor(OneWire * input_pin){ sensor = new DallasTemperature(input_pin); sensor->begin();}
    
    float read_temp();
};
