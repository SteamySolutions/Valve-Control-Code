#include <temp-sensor.hh>

float TempSensor::read_temp(){
  int V_in = analogRead(this->in_pin);
  //float R_therm = SERIES_RESISTOR * ((ADC_MAX / (float)V_in)*(3.3/5.0) - 1);
  float R_therm = SERIES_RESISTOR * ((ADC_MAX / (float)V_in) - 1);

  // Steinhart-Hart equation to convert resistance to temperature
  float temp;
    //temp = (1.0 / (A + (B*log_R_therm) + (C*log_R_therm*log_R_therm*log_R_therm)));
    temp = 1.0 / ((1.0 / ((float)NOMINAL_TEMPERATURE + 273.15)) + (1.0 / (float)B_COEFFICIENT) * log(R_therm / (float)NOMINAL_RESISTANCE));
    temp = temp - 273.15;
    temp = ((temp * 9.0) / 5.0) + 32.0; 
  
  return temp;
}