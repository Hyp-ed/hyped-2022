#pragma once

#include "logger.hpp"
#include "utils.hpp"

#include <cstdint>
#include <memory>

#include <utils/concurrent/barrier.hpp>

namespace hyped::utils {

class System {
 public:
  enum class RunKind { kOfficial, kOutside, kElevator, kStationary };
  struct Config {
    std::string client_config_path;
    std::string imu_config_path;
    std::string keyence_config_path;
    std::string temperature_config_path;
    std::string fake_trajectory_config_path;
    std::string bms_config_path;
    std::string brakes_config_path;
    Logger::Level log_level;
    Logger::Level log_level_propulsion;
    Logger::Level log_level_navigation;
    Logger::Level log_level_sensors;
    Logger::Level log_level_state_machine;
    Logger::Level log_level_telemetry;
    Logger::Level log_level_brakes;
    bool use_fake_trajectory;
    bool use_fake_batteries;
    bool use_fake_batteries_fail;
    bool use_fake_temperature;
    bool use_fake_temperature_fail;
    bool use_fake_brakes;
    bool use_fake_controller;
    bool use_fake_high_power;
    int8_t axis;
    int8_t run_id;
    RunKind run_kind;
  };
  static void parseArgs(int argc, char *argv[]);
  static System &getSystem();
  static Logger &getLogger();

  /**
   * Register custom signal handler for CTRL+C to make system exit gracefully
   */
  static bool setExitFunction();

  int8_t getRunId() const;
  bool isRunning() const;
  void stop();

  const Config config_;

 private:
  utils::concurrent::Lock lock;
  bool running_;
  Logger log_;

  System(const Config &config);
  inline static std::unique_ptr<System> system_;

  static void interruptHandler(int);
  static void segfaultHandler(int);

  System() = delete;
  NO_COPY_ASSIGN(System)
};

}  // namespace hyped::utils
