#include <Arduino.h>

#include "flow-sensor.hh"

FlowSensor::FlowSensor(const int isensor_pin, const int iticks_per_liter, const int imspt): sensor_pin(isensor_pin), ticks_per_liter(iticks_per_liter), milliseconds_per_update(imspt) {
  
}

float FlowSensor::get_flow_rate() {
  //noInterrupts();
  if(millis() - last_tick_time > milliseconds_per_update)
    this->liters_per_second = 0;
  //interrupts();
  return this->liters_per_second;
}

void FlowSensor::check() {
  int pin_state = digitalRead(sensor_pin);
  if(pin_state == HIGH && last_state == LOW)
    this->tick();

  last_state = pin_state;
}

void FlowSensor::tick() {
  uint32_t time = millis();
  if(time - last_tick_time > milliseconds_per_update) {
    liters_per_second = 0;
    ticks = 1;
    last_tick_time = time;
    last_update_time = time;
    return;
  }

  if(time - last_update_time > milliseconds_per_update) {
    float elapsed = ((float)time - (float)last_update_time) / 1000.0;
    float liters = (float)ticks / ticks_per_liter;

    last_tick_time = time;
    last_update_time = time;

    liters_per_second = liters / elapsed;

    ticks = 0;
    return;
  }

  ticks++;
  last_tick_time = time;
}