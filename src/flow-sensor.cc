#include <Arduino.h>

#include "flow-sensor.hh"

FlowSensor::FlowSensor(const int isensor_pin, const int iticks_per_liter, const int imspt): sensor_pin(isensor_pin), ticks_per_liter(iticks_per_liter), milliseconds_per_update(imspt) {
  last_update_time = millis();
}

float FlowSensor::get_flow_rate() {
  //noInterrupts();
  update();
  return this->liters_per_second;
}

void FlowSensor::check() {
  int pin_state = digitalRead(sensor_pin);
  if(pin_state && !last_state)
    this->tick();

  last_state = pin_state;
}

void FlowSensor::tick() {
  ticks++;
}

void FlowSensor::update() {
  unsigned long time = millis();
  unsigned long delta_t = time - last_update_time;
  if(delta_t == 0) return;
  double rate = (double)ticks / ticks_per_liter / delta_t * 1000.;
  liters_per_second = rate;
  last_update_time = time;
  ticks = 0;
}