#include <Wire.h>
#include <valve.hh>

Valve::Valve(int pin_num, int max_pwm, int min_pwm, Adafruit_PWMServoDriver& idriver){
  pin = pin_num;
  this->max_pwm = max_pwm;
  this->min_pwm = min_pwm;
  this->driver = &idriver;
};

void Valve::open(int angle){
  this->driver->setPWM(this->pin, 0, this->map_angle(angle));
};

void Valve::close(){
  this->driver->setPWM(this->pin, 0, this->min_pwm);
};

int temp_to_hangle(int set_temp, int h_temp, int c_temp){
  int h_angle = ((set_temp - c_temp) / (h_temp - c_temp)) * 90;
  
  return h_angle;
}