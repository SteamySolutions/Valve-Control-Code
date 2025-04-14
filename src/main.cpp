#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Arduino.h"
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
  hvalve = new Valve(0, 270, 450, driver);
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

Pid flow_pid(.3, 0.1, 0);

void loop() {
  // put your main code here, to run repeatedly:

  hotflow->check();
  coldflow->check();
  outflow->check();
  
  if(millis() - t0 > 1000) {
    t0 = millis();
    double hot_proportion = hvalve->temp_to_hangle(90, hot->read_temp(), cold->read_temp()) / 90.;
    double cold_proportion = 1 - hot_proportion;

    double hfr = hotflow->get_flow_rate();
    double cfr = coldflow->get_flow_rate();

    double maxfr = max(hfr, cfr);

    hfr /= maxfr;
    cfr /= maxfr;

    double flow_bias = hfr - cfr; // 0 -> flow is almost equal, >0 -> flow is biased towards hot, <0 -> flow is biased towards cold
    double flow_pbias = hot_proportion - cold_proportion;
    double flow_error = flow_pbias - flow_bias;

    double output = flow_pid.step(flow_error, 1);

    double houtput, coutput;
    if(output > 0) {
      houtput = 1.;
      coutput = 1. - output;
    } else {
      coutput = 1.;
      houtput = 1. + output;
    }

    hvalve->open(houtput * 90);
    cvalve->open(coutput * 90);
    

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
