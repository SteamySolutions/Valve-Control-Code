#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

//Create the servo driver object that will do all the "hard" communication for us
Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver();

// Maximum and minimum PWM frequency decides how far the servo turns
// This should be 0-90 degrees but need to test
const int MAX_PWM_FREQ = 420;
const int MIN_PWM_FREQ = 290;

// Doesn't get the owm freq per se but maps the servo angle 0-90 to the drivers pwm output 0-4095
// where 0 is always low and 4095 is always on (I think still need to test)
double map_angle(double angle){
  return (angle * (MAX_PWM_FREQ-MIN_PWM_FREQ)) / 90.0 - MIN_PWM_FREQ;
  //return map(angle, 0, 90, 0, 4096);
}

void setup() {
  // Set serial port (i2c) serial data transmission rate
  Serial.begin(9600);

  driver.begin();

  driver.setOscillatorFrequency(27000000);
  driver.setPWMFreq(50);  // This is the maximum PWM frequency

  Wire.setClock(400000);
}

void loop() {
  // put your main code here, to run repeatedly:
  // driver.setPWM(0, 0, map_angle(90));
  // delay(1000);

  for(int i = 290; i < 420; i++){
    driver.setPWM(0, 0, i);
    driver.setPWM(3, 0, i);
  }

  // driver.setPWM(0, 0, 600);
  delay(500);

  for(int i = 400; i > 290; i--){
    driver.setPWM(0, 0, i);
    driver.setPWM(3, 0, i);
  }

  //driver.setPWM(0, 0, map_angle(23));
  //delay(1000);

  //driver.setPWM(0, 0, map_angle(45));
  //delay(1000);

  //driver.setPWM(0, 0, map_angle(68));
  //delay(1000);

  //driver.setPWM(0, 0, map_angle(0));
  //delay(1000);

  //driver.setPWM(0, 0, 300);
  delay(500);

}