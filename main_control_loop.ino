#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define THERMISTOR_PIN A0  // Analog pin for thermistor
#define SERIES_RESISTOR 50000  // 50kΩ resistor value
#define NOMINAL_RESISTANCE 50000  // 50kΩ at 25°C
#define NOMINAL_TEMPERATURE 25  // Nominal temperature (25°C)
#define B_COEFFICIENT 3950  // Beta coefficient (varies by thermistor)
#define ADC_MAX 1023.0  // 10-bit ADC resolution

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

class temp_sensor{
    int in_pin;
  public:
    temp_sensor(int input_pin){
      this->in_pin = input_pin;
    }

    read_temp(){
      int adcValue = analogRead(THERMISTOR_PIN);
      float resistance = SERIES_RESISTOR / ((ADC_MAX / adcValue) - 1);
    
      // Steinhart-Hart equation to convert resistance to temperature
      float temperature;
      temperature = resistance / NOMINAL_RESISTANCE;      // (R/Ro)
      temperature = log(temperature);                     // ln(R/Ro)
      temperature /= B_COEFFICIENT;                       // 1/B * ln(R/Ro)
      temperature += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
      temperature = 1.0 / temperature;                    // Invert
      temperature -= 273.15;                              // Convert to Celsius
    }
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
