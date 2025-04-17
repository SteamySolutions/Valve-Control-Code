#pragma once

#include <Wire.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class TempSensor{
  DallasTemperature * sensor;

  public:
    TempSensor(OneWire * input_pin){ sensor = new DallasTemperature(input_pin); sensor->begin(); sensor->setWaitForConversion(false); }
    
    void request();
    float read_temp();
    float read_tempC();
};
