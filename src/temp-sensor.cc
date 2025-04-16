#include <temp-sensor.hh>

void TempSensor::request(){
  sensor->requestTemperatures();
}
float TempSensor::read_temp(){
  return sensor->getTempFByIndex(0);
}