#pragma once

class FlowSensor {
  public:
  FlowSensor(const int isensor_pin, const int ticks_per_liter, const int imspt);

  void tick();
  /**
   * @brief Check if the sensor needs to be ticked, and if so, tick it
   * 
   */
  void check();
  float get_flow_rate();

  const int sensor_pin;
  const int ticks_per_liter;
  const int milliseconds_per_update;
  int last_update_time;
  int last_tick_time;
  float liters_per_second {0};
  int ticks {0};

  int last_state {LOW};
  
};
