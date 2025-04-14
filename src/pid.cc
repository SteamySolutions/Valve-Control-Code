#include "pid.hh"

double clamp(double value, double min, double max) {
  if (value < min) {
    return min;
  } else if (value > max) {
    return max;
  }
  return value;
}

double Pid::step(double current, double time_elapsed) {
  double derivative = -k_d * (current - previous) / time_elapsed;
  integral += (target - current) * time_elapsed * k_i;
  double proportional = k_p * (target - current);

  previous = current;

  integral = clamp(integral, -1, 1);

  return proportional + integral + derivative;
}

void Pid::set_target(double itarget, double current) {
  target = itarget;
}

Pid::Pid(const double ik_p, const double ik_i, const double ik_d): k_p(ik_p), k_i(ik_i), k_d(ik_d){}
