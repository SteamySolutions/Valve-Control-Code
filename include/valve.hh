#pragma once
#include <Adafruit_PWMServoDriver.h>

class Valve{
  private:
    int pin;
    int max_pwm;
    int min_pwm;

    Adafruit_PWMServoDriver* driver;

    int map_angle(int angle){
      return map(angle, 0, 90, this->min_pwm, this->max_pwm);
    };

  public:
  Valve(int pin_num, int max_pwm, int min_pwm, Adafruit_PWMServoDriver& idriver);

  void open(int angle);
  void close();

};