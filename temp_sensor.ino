#define THERMISTOR_PIN A0  // Analog pin for thermistor
#define SERIES_RESISTOR 50000  // 50kΩ resistor value
#define NOMINAL_RESISTANCE 50000  // 50kΩ at 25°C
#define NOMINAL_TEMPERATURE 25  // Nominal temperature (25°C)
#define B_COEFFICIENT 3950  // Beta coefficient (varies by thermistor)
#define ADC_MAX 1023.0  // 10-bit ADC resolution

void setup() {
    Serial.begin(9600);
}

void loop() {
    int adcValue = analogRead(THERMISTOR_PIN);
    float resistance = SERIES_RESISTOR / ((ADC_MAX / adcValue) - 1);
    
    // Steinhart-Hart equation to convert resistance to temperature
    float temperature;
    temperature = resistance / NOMINAL_RESISTANCE;      // (R/Ro)
    temperature = log(temperature);                     // ln(R/Ro)
    temperature /= B_COEFFICIENT;                       // 1/B * ln(R/Ro)
    temperature += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
    temperature = 1.0 / temperature;                    // Invert
    temperature -= 273.15;                              // Convert to Celsius

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    delay(1000);  // Read every second
}