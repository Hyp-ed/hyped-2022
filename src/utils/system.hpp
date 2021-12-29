#pragma once

#include "logger.hpp"
#include "utils.hpp"

#include <cstdint>

#include <utils/concurrent/barrier.hpp>

namespace hyped {

using utils::concurrent::Barrier;

namespace utils {

// forward declaration
class Config;

class System {
 public:
  static void parseArgs(int argc, char *argv[]);
  static System &getSystem();
  static Logger &getLogger();

  /**
   * Register custom signal handler for CTRL+C to make system exit gracefully
   */
  static bool setExitFunction();

  // runtime arguments to configure the whole system
  int8_t verbose;
  int8_t verbose_motor;
  int8_t verbose_nav;
  int8_t verbose_sensor;
  int8_t verbose_state;
  int8_t verbose_tlm;
  int8_t verbose_brakes;

  int8_t debug;
  int8_t debug_motor;
  int8_t debug_nav;
  int8_t debug_sensor;
  int8_t debug_state;
  int8_t debug_tlm;
  int8_t debug_brakes;

  // Fake System variables below
  bool fake_trajectory;
  bool fake_batteries;
  bool fake_temperature;
  bool fake_brakes;
  bool fake_motors;

  // Fake Fail System variables below
  bool fake_imu_fail;
  bool fake_batteries_fail;
  bool fake_keyence_fail;
  bool fake_temperature_fail;

  // sensor test variables below
  bool battery_test;
  bool fake_highpower;

  // Navigation System IDs
  int8_t imu_id;
  int8_t run_id;
  // Navigation IMU axis
  int8_t axis;
  // Navigation kind of run
  bool official_run;
  bool elevator_run;
  bool stationary_run;
  bool outside_run;
  // Write acc,vel,pos to file
  bool enable_nav_write;

  // Telemetry
  bool telemetry_off;

  // barriers
  /**
   * @brief Barrier used by navigation and motor control modules on state machine transition
   *        to accelerating state. Navigation must finish calibration before motors start spinning.
   */
  Barrier navigation_motors_sync_ = Barrier(2);
  bool running_;

  char config_file[250];
  Config *config;

 private:
  Logger *log_;
  System() = delete;

  /**
   * @brief Construct a new System object, parameters used by getopt_long to set verbose and debug
   *
   * @param argc argument count
   * @param argv argument array
   */
  System(int argc, char *argv[]);
  ~System();
  static System *system_;

  // macro to help implemet singleton
  NO_COPY_ASSIGN(System)
};

}  // namespace utils
}  // namespace hyped
