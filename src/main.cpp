#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Arduino.h"
#include "temp-sensor.hh"
#include "valve.hh"
#include "flow-sensor.hh"
#include "pid.hh"

// void read_serial(){
//   if(Serial.available() > 0){
//     unsigned char command = Serial.read();
//     if(command = 'E'){
//       delay(100);
//       int newTemp = Serial.parseInt();
//       Serial.println(newTemp);
//     }
//     else if (command = 'T'){
//       if(shower){
//         shower = false;
//         hvalve->close();
//         cvalve->close();
//       }  
//       else{
//         shower = true;
//       }
//     }
//   }
//   delay(100);
// }

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

bool shower;

void tick_hot() {
  hotflow->tick();
}

void tick_cold() {
  coldflow->tick();
}
void tick_mix() {
  outflow->tick();
}

void setup() {
    // Set serial port (i2c) serial data transmission rate
  Serial.begin(9600);

  driver.begin();

  driver.setOscillatorFrequency(27000000);
  driver.setPWMFreq(50);  // This is the maximum PWM frequency

  Wire.setClock(400000);

  // define hot and cold valve objects
  hvalve = new Valve(0, 280, 460, driver);
  cvalve = new Valve(1, 309, 435, driver);

  // define hot, cold, and out temperature sensor objects
  hot = new TempSensor(2, 45000);
  cold = new TempSensor(1);
  out = new TempSensor(0, 47000);

  hotflow = new FlowSensor(8, 553, 250);
  coldflow = new FlowSensor(4, 553, 250);
  outflow = new FlowSensor(7, 553, 250);
}

unsigned long t0 = millis();

void loop() {
  // put your main code here, to run repeatedly:

  hotflow->check();
  coldflow->check();
  outflow->check();
  
  int hot_angle;

  if(millis() - t0 > 1000) {
    t0 = millis();
    hot_angle = hvalve->temp_to_hangle(90, hot->read_temp(), cold->read_temp());
    //Serial.println(hot_angle);
    hvalve->open(90);
    delay(1000);
    hvalve->open(0);
    delay(1000);
    //cvalve->open(90-hot_angle);

    Serial.print("Hot Temperature: ");
    Serial.print(hot->read_temp());
    Serial.print("F ");
    Serial.print("Hot flow: ");
    Serial.print(hotflow->get_flow_rate());
    Serial.println("L/s");


    Serial.print("Cold Temperature: ");
    Serial.print(cold->read_temp());
    Serial.print("F ");
    Serial.print("Cold flow: ");
    Serial.print(coldflow->get_flow_rate());
    Serial.println("L/s");

    Serial.print("Out Temperature: ");
    Serial.print(out->read_temp());
    Serial.print("F ");
    Serial.print("Out flow: ");
    Serial.print(outflow->get_flow_rate());
    Serial.println("L/s");
  }
}
