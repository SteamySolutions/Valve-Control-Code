#pragma once

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
