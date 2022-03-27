#pragma once

#include "logger.hpp"
#include "utils.hpp"

#include <cstdint>
#include <memory>

#include <rapidjson/document.h>

#include <utils/concurrent/barrier.hpp>

namespace hyped::utils {

class System {
 public:
  struct Config {
    std::string client_config_path;
    std::string imu_config_path;
    std::string temperature_config_path;
    std::string fake_trajectory_config_path;
    std::string bms_config_path;
    std::string brakes_config_path;
    std::string debugger_config_path;
    Logger::Level log_level;
    Logger::Level log_level_brakes;
    Logger::Level log_level_navigation;
    Logger::Level log_level_propulsion;
    Logger::Level log_level_sensors;
    Logger::Level log_level_state_machine;
    Logger::Level log_level_telemetry;
    Logger::Level log_level_debugger;
    bool use_fake_trajectory;
    bool use_fake_batteries;
    bool use_fake_batteries_fail;
    bool use_fake_temperature;
    bool use_fake_temperature_fail;
    bool use_fake_brakes;
    bool use_fake_controller;
    std::uint8_t axis;
    std::uint64_t run_id;
  };
  System(const Config &config);
  static void parseArgs(const int argc, const char *const *const argv);
  static std::uint64_t newRunId();
  static System &getSystem();
  static Logger &getLogger();

  /**
   * Register custom signal handler for CTRL+C to make system exit gracefully
   */
  static bool setExitFunction();

  bool isRunning();
  void stop();

  const Config config_;

 private:
  utils::concurrent::Lock lock_;
  bool running_;
  Logger log_;

  inline static std::unique_ptr<System> system_;

  static void interruptHandler(int);
  static void segfaultHandler(int);

  System() = delete;
  NO_COPY_ASSIGN(System)
};

}  // namespace hyped::utils
