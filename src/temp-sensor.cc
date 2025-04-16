#include <temp-sensor.hh>

float TempSensor::read_temp(){
  sensor->requestTemperatures();
  return sensor->getTempFByIndex(0);
}