volatile int pulseCount = 0;  // Pulse counter
const int sensorPin = 2;      // Digital pin with interrupt (e.g., 2 or 3 on Uno)
unsigned long lastTime = 0;   // For timing
float flowRate;               // Flow rate in L/hour
const float pulsesPerLiter = 553;  // Example calibration factor (check datasheet)

void setup() {
  pinMode(sensorPin, INPUT_PULLUP);  // Enable internal pull-up for NPN
  attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, FALLING);  // Interrupt on falling edge
  Serial.begin(9600);
  lastTime = millis();
}

void loop() {
  if (millis() - lastTime >= 1000) {  // Calculate every second
    noInterrupts();  // Disable interrupts briefly to read pulseCount safely
    int pulses = pulseCount;
    pulseCount = 0;  // Reset counter
    interrupts();    // Re-enable interrupts

    // Calculate flow rate (L/hour): pulses per second * (3600 / pulses per liter)
    flowRate = pulses / pulsesPerLiter;
    Serial.print("Flow Rate: ");
    Serial.print(flowRate);
    Serial.println(" L/sec");

    lastTime = millis();
  }
}

void countPulse() {
  pulseCount++;  // Increment pulse count on each falling edge
}

class FlowSensor {
  public:
  FlowSensor(const int isensor_pin): sensor_pin(isensor_pin) {
    
  }

  const int sensor_pin;
  int ticks = 0;
  
};