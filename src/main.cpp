#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <OneWire.h>
#include "Arduino.h"
#include "HardwareSerial.h"
#include "temp-sensor.hh"
#include "valve.hh"
#include "flow-sensor.hh"
#include "pid.hh"

//Create the servo driver object that will do all the "hard" communication for us
Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver();

Valve * hvalve;
Valve * cvalve;
Valve * bvalve;

OneWire hwire(2);
OneWire cwire(3);
OneWire owire(5);

TempSensor * hot;
TempSensor * cold;
TempSensor * out; 

FlowSensor * hotflow;
FlowSensor * coldflow;
FlowSensor * outflow;

Pid flow_pid(.1, 0.05, 0);

int set_temp;

bool shower;

void read_serial(){
  if(Serial.available() > 0){
    unsigned char command = Serial.read();
    if(command == 'E'){
      delay(100);
      int newTemp = Serial.parseInt();
      set_temp = newTemp;
      Serial.println(newTemp);
    }
    else if (command == 'T'){
      if(shower){
        //stop updating the motor angle
        shower = false;

        //close the hot and cold valves
        hvalve->close();
        cvalve->close();

        //Put the motor diriver to sleep so it's not drawing power
        delay(1000);
        driver.sleep();
      }  
      else{
        shower = true;
        driver.wakeup();
        delay(1000);
      }
    }
  }
  delay(100);
}

void write_serial(char code, int val1, int val2){
  //Write the intial code so RPi knows what it is getting
  Serial.write(code);
  delay(500);
  if(code == 'A'){
    Serial.write(val1);
  }
  else if(code == 'B'){
    Serial.write(val1);
    delay(500);
    Serial.write(val2);
  }
}

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
  Serial.println("Begin Set up");

  driver.begin();

  driver.setOscillatorFrequency(27000000);
  driver.setPWMFreq(50);  // This is the maximum PWM frequency

  Wire.setClock(400000);

  //vars for the control loop from communication protocol
  shower = true;
  set_temp = 80;

  Serial.println("Define the valves");
    // define hot and cold valve objects
  hvalve = new Valve(0, 280, 460, driver);
  cvalve = new Valve(1, 309, 435, driver);

  Serial.println("Define temp sensors");
    // define hot, cold, and out temperature sensor objects
  //Yellow wire
  hot = new TempSensor(&hwire);
  //green wire
  cold = new TempSensor(&cwire);
  //Brown wire
  out = new TempSensor(&owire);

  hotflow = new FlowSensor(8, 553, 250);
  coldflow = new FlowSensor(4, 553, 250);
  outflow = new FlowSensor(7, 553, 250);
}

unsigned long t0 = millis();

double clamp(double, double, double);

void loop() {
  delay(1000);
  // put your main code here, to run repeatedly:
  
  read_serial();

  hotflow->check();
  coldflow->check();
  outflow->check();
  
  if(millis() - t0 > 1000) {
    t0 = millis();
    double hot_proportion = hvalve->temp_to_hangle(set_temp, hot->read_temp(), cold->read_temp()) / 90.;
    double cold_proportion = 1 - hot_proportion;

    double hfraw = hotflow->get_flow_rate();
    double cfraw = coldflow->get_flow_rate();

    double hfr = hfraw;
    double cfr = cfraw;

    double maxfr = max(hfr, cfr);

    if(maxfr > 0) {
      hfr /= maxfr;
      cfr /= maxfr;
    }
    else {
      maxfr = 1;
      hfr /= maxfr;
      cfr /= maxfr;
    };


    Serial.println(hfr);

    double flow_bias = hfr - cfr; // 0 -> flow is almost equal, >0 -> flow is biased towards hot, <0 -> flow is biased towards cold
    double flow_pbias = hot_proportion - cold_proportion;
    double flow_error = flow_pbias - flow_bias;

    flow_pid.set_target(flow_pbias, 0);

    double output = flow_pid.step(flow_bias, 1);

    output = clamp(output, -1, 1);

    Serial.println(output);

    double houtput, coutput;
    if(output > 0) {
      houtput = 1.;
      coutput = 1. - output;
    } else {
      coutput = 1.;
      houtput = 1. + output;
    }

    if(shower){
      hvalve->open(houtput * 90);
      cvalve->open(coutput * 90);
    }
    

    Serial.print("Hot Temperature: ");
    Serial.print(hot->read_temp());
    Serial.println("F ");
    delay(1000);
    Serial.print("Hot flow: ");
    Serial.print(hfraw);
    Serial.println("L/s");


    Serial.print("Cold Temperature: ");
    Serial.print(cold->read_temp());
    Serial.println("F ");
    delay(1000);
    Serial.print("Cold flow: ");
    Serial.print(cfraw);
    Serial.println("L/s");

    Serial.print("Out Temperature: ");
    Serial.print(out->read_temp());
    Serial.println("F ");
    Serial.print("Out flow: ");
    Serial.print(outflow->get_flow_rate());
    Serial.println("L/s");

    if(Serial.available() > 0){
      read_serial();
    }
  }
}
