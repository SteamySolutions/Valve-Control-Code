#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

//Create the servo driver object that will do all the "hard" communication for us
Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver();

//Maximum and minimum PWM frequency decides how far the servo turns, for our uses 0-90 degrees
const int MAX_PWM[] = {420, 460, 420};
const int MIN_PWM[] = {290, 270, 290};

//Maps angles to pwm frequencies
int map_angle(int valve_num, int angle){
  return map(angle, 0, 90, MIN_PWM[valve_num], MAX_PWM[valve_num]);
}

void open_valve(int valve_num, int angle){
  driver.setPWM(valve_num, 0, map_angle(valve_num, angle));
}

void close_valve(int valve_num){
  driver.setPWM(valve_num, 0, MIN_PWM[valve_num]);
}

void turn_control_off(){
  driver.sleep();
}

void turn_control_on(){
  driver.wakeup();
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
  open_valve(0, 60);
  open_valve(1, 30);
  delay(1000);
  close_valve(0);
  close_valve(1);
  delay(1000);

  // for(int i = 290; i < 420; i++){
  //   driver.setPWM(0, 0, i);
  //   driver.setPWM(3, 0, i);
  // }

  //delay(500)

  // for(int i = 400; i > 290; i--){
  //   driver.setPWM(0, 0, i);
  //   driver.setPWM(3, 0, i);
  // }
}