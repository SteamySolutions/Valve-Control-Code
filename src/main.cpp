#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "temp-sensor.hh"
#include "valve.hh"

//Create the servo driver object that will do all the "hard" communication for us
Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver();

void setup() {
    // Set serial port (i2c) serial data transmission rate
  Serial.begin(9600);

  driver.begin();

  driver.setOscillatorFrequency(27000000);
  driver.setPWMFreq(50);  // This is the maximum PWM frequency

  Wire.setClock(400000);

  // put your setup code here, to run once:
  Valve * hvalve = new Valve(0, 309, 435, driver);
  Valve * cvalve = new Valve(1, 270, 450, driver);
  Valve * bvalve = new Valve(2, 310, 500, driver);
}

void loop() {
  // put your main code here, to run repeatedly:

}
