#include <temp-sensor.hh>

float TempSensor::read_temp(){
  int adcValue = analogRead(this->in_pin);
  float resistance = SERIES_RESISTOR / ((ADC_MAX / adcValue) - 1);

  // Steinhart-Hart equation to convert resistance to temperature
  float temperature;
  temperature = resistance / NOMINAL_RESISTANCE;      // (R/Ro)
  temperature = log(temperature);                     // ln(R/Ro)
  temperature /= B_COEFFICIENT;                       // 1/B * ln(R/Ro)
  temperature += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
  temperature = 1.0 / temperature;                    // Invert
  temperature -= 273.15;                              // Convert to Celsius

  return temperature;
}