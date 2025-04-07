#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "temp-sensor.hh"
#include "valve.hh"

//Create the servo driver object that will do all the "hard" communication for us
Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver();

Valve * hvalve;
Valve * cvalve;
Valve * bvalve;

TempSensor * hot;
TempSensor * cold;
TempSensor * out; 

bool first = true;

void setup() {
    // Set serial port (i2c) serial data transmission rate
  Serial.begin(9600);

  driver.begin();

  driver.setOscillatorFrequency(27000000);
  driver.setPWMFreq(50);  // This is the maximum PWM frequency

  Wire.setClock(400000);

  // put your setup code here, to run once:
  //hvalve = new Valve(0, 309, 435, driver);
  cvalve = new Valve(1, 309, 435, driver);
  //cvalve = new Valve(1, 270, 450, driver);
  hvalve = new Valve(0, 270, 450, driver);
  //bvalve = new Valve(2, 310, 500, driver);

  hot = new TempSensor(2, 45000);
  cold = new TempSensor(1);
  out = new TempSensor(0, 47000);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  int hot_angle;
  //if(first){
    hot_angle = hvalve->temp_to_hangle(90, hot->read_temp(), cold->read_temp());
    first = false;
    Serial.println(hot_angle);
    hvalve->open(hot_angle);
    cvalve->open(90-hot_angle);
  //}

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
