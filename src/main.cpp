#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <OneWire.h>
#include "Arduino.h"
#include "HardwareSerial.h"
#include "temp-sensor.hh"
#include "valve.hh"
#include "flow-sensor.hh"
#include "pid.hh"

#define DEBUG false

// Tuning parameter for fixing the tuned temperature
const double TEMPERATURE_OFFSET = 7;

//Create the servo driver object that will do all the "hard" communication for us
Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver();

Valve * hvalve;
Valve * cvalve;
Valve * bvalve;

OneWire hwire(3);
OneWire cwire(2);
OneWire owire(13);

TempSensor * hot;
TempSensor * cold;
TempSensor * out; 

FlowSensor * hotflow;
FlowSensor * coldflow;
FlowSensor * outflow;

Pid flow_pid(.1, 0.2, 0);

int set_temp;

bool shower;

void read_serial(){
  if(Serial.available() > 0){
    unsigned char command = Serial.read();
    if(command == 'S'){
      Serial.setTimeout(30);
      int newTemp = atoi(Serial.readStringUntil('\n').c_str());
      set_temp = newTemp;
    }
    else if (command == 'T'){
      if(shower){
        //stop updating the motor angle
        shower = false;

        //close the hot and cold valves
        hvalve->close();
        cvalve->close();

        flow_pid.update_kff();

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

  Serial.flush();
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

  driver.begin();

  driver.setOscillatorFrequency(27000000);
  driver.setPWMFreq(50);  // This is the maximum PWM frequency

  Wire.setClock(400000);

  //vars for the control loop from communication protocol
  shower = false;
  set_temp = 100;

    // define hot and cold valve objects
  hvalve = new Valve(0, 280, 460, driver);
  cvalve = new Valve(1, 309, 435, driver);

  // define hot, cold, and out temperature sensor objects
  hot = new TempSensor(&hwire);
  cold = new TempSensor(&cwire);
  out = new TempSensor(&owire);

  hotflow = new FlowSensor(8, 553, 250);
  coldflow = new FlowSensor(4, 553, 250);
  outflow = new FlowSensor(7, 553, 250);

  hot->request();
  cold->request();
  out->request();
}

unsigned long t0 = millis();

double clamp(double, double, double);

double last_out_temp = 70;

void loop() {
  hotflow->check();
  coldflow->check();
  outflow->check();
  
  if(millis() - t0 > 1000) {
    t0 = millis();
    float hot_temp = hot->read_temp();
    float cold_temp = cold->read_temp();
    float out_temp = out->read_temp();

    double hot_proportion = hvalve->temp_to_hangle(set_temp - TEMPERATURE_OFFSET, hot_temp, cold_temp) / 90.;
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


    //Serial.println(hfr);

    double flow_bias = hfr - cfr; // 0 -> flow is almost equal, >0 -> flow is biased towards hot, <0 -> flow is biased towards cold
    double flow_pbias = hot_proportion - cold_proportion;
    double flow_error = flow_pbias - flow_bias;

    flow_pid.set_target(flow_pbias, 0);

    double output = flow_pid.step(flow_bias, 1);

    output = clamp(output, -1, 1);


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
    
    if(DEBUG){
      Serial.print("Hot Temperature: ");
      Serial.print(hot_temp);
      Serial.println("F ");
      Serial.print("Hot flow: ");
      Serial.print(hfraw);
      Serial.println("L/s");


      Serial.print("Cold Temperature: ");
      Serial.print(cold_temp);
      Serial.println("F ");
      Serial.print("Cold flow: ");
      Serial.print(cfraw);
      Serial.println("L/s");

      Serial.print("Out Temperature: ");
    }

    if(out_temp >= 0) {
      Serial.println(out_temp);
      last_out_temp = out_temp;
    }
    else {
      if(hfr == 0 && cfr == 0)
        hfr = (cfr = 1);
      double hot_ratio = hfr / (hfr + cfr);
      double cold_ratio = cfr / (hfr + cfr);

      out_temp = hot_ratio * hot_temp + cold_ratio * cold_temp + TEMPERATURE_OFFSET;
      Serial.println(out_temp);
    }

    if(DEBUG){
      Serial.print("Out flow: ");
      Serial.print(outflow->get_flow_rate());
      Serial.println("L/s");
    }

    if(Serial.available() > 0){
      read_serial();
    }
    
    hot->request();
    out->request();
    cold->request();
  }
}
