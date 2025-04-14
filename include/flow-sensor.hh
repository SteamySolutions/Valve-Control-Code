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
  void update();

  const int sensor_pin;
  const int ticks_per_liter;
  const int milliseconds_per_update;
  unsigned long last_update_time;
  unsigned long last_tick_time;
  float liters_per_second {0};
  long ticks {0};

  int last_state {0};
  
};
