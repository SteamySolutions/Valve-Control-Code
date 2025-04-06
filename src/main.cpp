#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "temp-sensor.hh"

//Create the servo driver object that will do all the "hard" communication for us
Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver();

class valve{
  private:
    int pin;
    int max_pwm;
    int min_pwm;

    int map_angle(int angle){
      return map(angle, 0, 90, this->min_pwm, this->max_pwm);
    };

  public:
  valve(int pin_num, int max_pwm, int min_pwm){
    pin = pin_num;
    this->max_pwm = max_pwm;
    this->min_pwm = min_pwm;
  };

  void open_valve(int angle){
    driver.setPWM(this->pin, 0, this->map_angle(angle));
  };

  void close_valve(){
    driver.setPWM(this->pin, 0, this->min_pwm);
  };

};

void setup() {
    // Set serial port (i2c) serial data transmission rate
  Serial.begin(9600);

  driver.begin();

  driver.setOscillatorFrequency(27000000);
  driver.setPWMFreq(50);  // This is the maximum PWM frequency

  Wire.setClock(400000);

  // put your setup code here, to run once:
  valve * hvalve = new valve(0, 309, 435);
  valve * cvalve = new valve(1, 270, 450);
  valve * bvalve = new valve(2, 310, 500);
}

void loop() {
  // put your main code here, to run repeatedly:

}
