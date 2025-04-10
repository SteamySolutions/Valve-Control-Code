#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


class Pid {
    public:
    Pid(const double ik_p, const double ik_i, const double ik_d);

    double step(double current, double time_elapsed);
    void set_target(double itarget, double current);

    private:
    const double k_p;
    const double k_i;
    const double k_d;

    double target {0.0};
    double previous {0.0};
    double integral {0.0};
};

double Pid::step(double current, double time_elapsed) {
    double derivative = -k_d * (current - previous) / time_elapsed;
    integral += (current - target) * time_elapsed * k_i;
    double proportional = k_p * (current - target);

    previous = current;

    return proportional + integral + derivative;
}

void Pid::set_target(double itarget, double current) {
    target = itarget;
    previous = current;
    integral = 0;
}

Pid::Pid(const double ik_p, const double ik_i, const double ik_d): k_p(ik_p), k_i(ik_i), k_d(ik_d){}
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

//valve * hvalve = new valve(0, 420, 290);
valve * hvalve = new valve(0, 309, 435);
//valve * cvalve = new valve(1, 460, 270);
valve * cvalve = new valve(1, 270, 450);
valve * bvalve = new valve(2, 310, 500);
//valve * bvalve = new valve(2, 500, 270);


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

int temp_to_hangle(int set_temp, int h_temp, int c_temp){
  int h_angle = ((set_temp - c_temp) / (h_temp - c_temp)) * 90;
  
  return h_angle;
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
  hvalve->open_valve(90);
  cvalve->open_valve(90);
  bvalve->open_valve(90);
  delay(1000);
  hvalve->close_valve();
  cvalve->close_valve();
  bvalve->close_valve();
  delay(1000);
}