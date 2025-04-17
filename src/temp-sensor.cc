#include <temp-sensor.hh>

void TempSensor::request(){
  sensor->requestTemperatures();
}
float TempSensor::read_temp(){
  return sensor->getTempFByIndex(0);
}
float TempSensor::read_tempC(){
  return sensor->getTempCByIndex(0);
}

