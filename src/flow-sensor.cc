#include <Arduino.h>

#include "flow-sensor.hh"

FlowSensor::FlowSensor(const int isensor_pin, const int iticks_per_liter, const int imspt): sensor_pin(isensor_pin), ticks_per_liter(iticks_per_liter), milliseconds_per_update(imspt) {
  attachInterrupt(digitalPinToInterrupt(sensor_pin), [](){}, RISING);
}

float FlowSensor::get_flow_rate() {
  noInterrupts();
  if(millis() - last_tick_time > milliseconds_per_update)
    this->liters_per_second = 0;
  return this->liters_per_second;
  interrupts();
}

void FlowSensor::tick() {
  noInterrupts();
  uint32_t time = millis();
  if(time - last_tick_time > milliseconds_per_update) {
    liters_per_second = 0;
    ticks = 1;
    last_tick_time = time;
    last_update_time = time;
    interrupts();
    return;
  }

  if(time - last_update_time > milliseconds_per_update) {
    last_tick_time = time;
    last_update_time = time;

    float elapsed = ((float)time - last_update_time) / 1000.;
    float liters = (float)ticks / ticks_per_liter;

    liters_per_second = liters / elapsed;

    ticks = 0;
    interrupts();
    return;
  }

  ticks++;
  last_tick_time = time;
  interrupts();
}