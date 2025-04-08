#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "temp-sensor.hh"
#include "valve.hh"
#include "flow-sensor.hh"
#include "pid.hh"

//Create the servo driver object that will do all the "hard" communication for us
Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver();

Valve * hvalve;
Valve * cvalve;
Valve * bvalve;

TempSensor * hot;
TempSensor * cold;
TempSensor * out; 

FlowSensor * hotflow;
FlowSensor * coldflow;
FlowSensor * outflow;

void setup() {
    // Set serial port (i2c) serial data transmission rate
  Serial.begin(9600);

  driver.begin();

  driver.setOscillatorFrequency(27000000);
  driver.setPWMFreq(50);  // This is the maximum PWM frequency

  Wire.setClock(400000);

  // define hot and cold valve objects
  hvalve = new Valve(0, 270, 450, driver);
  cvalve = new Valve(1, 309, 435, driver);

  // define hot, cold, and out temperature sensor objects
  hot = new TempSensor(2, 45000);
  cold = new TempSensor(1);
  out = new TempSensor(0, 47000);

  hotflow = new FlowSensor(2);
  coldflow = new FlowSensor(3);
  outflow = new FlowSensor(4);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  int hot_angle;

  hot_angle = hvalve->temp_to_hangle(90, hot->read_temp(), cold->read_temp());
  //Serial.println(hot_angle);
  hvalve->open(hot_angle);
  cvalve->open(90-hot_angle);

  Serial.print("Hot Temperature: ");
  Serial.print(hot->read_temp());
  Serial.println("F");

  Serial.print("Cold Temperature: ");
  Serial.print(cold->read_temp());
  Serial.println("F");

  Serial.print("Out Temperature: ");
  Serial.print(out->read_temp());
  Serial.println("F");
  delay(1000);
}
