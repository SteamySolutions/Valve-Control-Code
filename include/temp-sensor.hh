#pragma once

#include <Wire.h>
#include <Arduino.h>

class TempSensor{
  int in_pin;

  int NOMINAL_RESISTANCE  = 50000;  // 50kΩ at 25°C
  int SERIES_RESISTOR  = 47000;  // 50kΩ resistor value
  int NOMINAL_TEMPERATURE = 25;  // Nominal temperature (25°C)
  int B_COEFFICIENT = 3950;  // Beta coefficient (varies by thermistor)
  int ADC_MAX = 1023.0;  // 10-bit ADC resolution


  public:
    TempSensor(int input_pin): in_pin(input_pin) {}
    TempSensor(int input_pin, int Nom_R):NOMINAL_RESISTANCE(Nom_R), in_pin(input_pin) {}

    float read_temp();
};
