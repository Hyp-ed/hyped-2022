#include "system.hpp"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <csignal>
#include <cstring>
#include <fstream>
#include <random>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

namespace hyped::utils {

/**
 * Static logger to be used before the system has been intialised and the logging
 * configuration is known.
 */
static Logger kInitialisationErrorLogger("SYSTEM", Logger::Level::kError);

System::System(const Config &config)
    : config_(config),
      running_(true),
      log_("SYSTEM", config.log_level)
{
}

void System::parseArgs(const int argc, const char *const *const argv)
{
  if (argc != 1) {
    kInitialisationErrorLogger.error(
      "found %d arguments but %d were expected; more arguments are not yet supported", argc, 1);
    exit(1);
  }
  std::ifstream input_stream(argv[0]);
  if (!input_stream.is_open()) {
    kInitialisationErrorLogger.error("failed to open config file at %s", argv[0]);
    exit(1);
  }
  rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
  rapidjson::Document document;
  document.ParseStream(input_stream_wrapper);
  if (document.HasParseError()) {
    kInitialisationErrorLogger.error("failed to parse config file at %s", argv[0]);
    exit(1);
  }
  if (!document.HasMember("system")) {
    kInitialisationErrorLogger.error("missing required field 'system' in configuration file at %s",
                                     argv[0]);
    exit(1);
  }
  const auto config_object = document["system"].GetObject();
  // Default values
  Config config;
  config.client_config_path          = std::string(argv[0]);
  config.imu_config_path             = std::string(argv[0]);
  config.keyence_config_path         = std::string(argv[0]);
  config.temperature_config_path     = std::string(argv[0]);
  config.fake_trajectory_config_path = std::string(argv[0]);
  config.bms_config_path             = std::string(argv[0]);
  config.brakes_config_path          = std::string(argv[0]);
  config.run_id                      = newRunId();
  // System log level
  if (config_object.HasMember("log_level")) {
    const auto log_level_raw = config_object["log_level"].GetInt();
    const auto log_level     = Logger::levelFromInt(log_level_raw);
    if (!log_level) {
      kInitialisationErrorLogger.error(
        "unknown value for system.log_level (%d) in config file at %s", log_level_raw, argv[0]);
      exit(1);
    }
    config.log_level = *log_level;
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.log_level' in config file at %s; using default value", argv[0]);
    config.log_level = Logger::Level::kInfo;
  }
  // Brakes log level
  if (config_object.HasMember("log_level_brakes")) {
    const auto log_level_raw = config_object["log_level_brakes"].GetInt();
    const auto log_level     = Logger::levelFromInt(log_level_raw);
    if (!log_level) {
      kInitialisationErrorLogger.error(
        "unknown value for system.log_level (%d) in config file at %s", log_level_raw, argv[0]);
      exit(1);
    }
    config.log_level_brakes = *log_level;
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.log_level_brakes' in config filet at %s; using default value",
      argv[0]);
    config.log_level_brakes = config.log_level;
  }
  // Navigation log level
  if (config_object.HasMember("log_level_navigation")) {
    const auto log_level_raw = config_object["log_level_navigation"].GetInt();
    const auto log_level     = Logger::levelFromInt(log_level_raw);
    if (!log_level) {
      kInitialisationErrorLogger.error(
        "unknown value for system.log_level (%d) in config file at %s", log_level_raw, argv[0]);
      exit(1);
    }
    config.log_level_navigation = *log_level;
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.log_level_navigation' in config filet at %s; using default "
      "value",
      argv[0]);
    config.log_level_navigation = config.log_level;
  }
  // Propulsion log level
  if (config_object.HasMember("log_level_propulsion")) {
    const auto log_level_raw = config_object["log_level_propulsion"].GetInt();
    const auto log_level     = Logger::levelFromInt(log_level_raw);
    if (!log_level) {
      kInitialisationErrorLogger.error(
        "unknown value for system.log_level (%d) in config file at %s", log_level_raw, argv[0]);
      exit(1);
    }
    config.log_level_propulsion = *log_level;
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.log_level_propulsion' in config filet at %s; using default "
      "value",
      argv[0]);
    config.log_level_propulsion = config.log_level;
  }
  // Sensors log level
  if (config_object.HasMember("log_level_sensors")) {
    const auto log_level_raw = config_object["log_level_sensors"].GetInt();
    const auto log_level     = Logger::levelFromInt(log_level_raw);
    if (!log_level) {
      kInitialisationErrorLogger.error(
        "unknown value for system.log_level (%d) in config file at %s", log_level_raw, argv[0]);
      exit(1);
    }
    config.log_level_sensors = *log_level;
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.log_level_sensors' in config filet at %s; using default value",
      argv[0]);
    config.log_level_sensors = config.log_level;
  }
  // State machine log level
  if (config_object.HasMember("log_level_state_machine")) {
    const auto log_level_raw = config_object["log_level_state_machine"].GetInt();
    const auto log_level     = Logger::levelFromInt(log_level_raw);
    if (!log_level) {
      kInitialisationErrorLogger.error(
        "unknown value for system.log_level (%d) in config file at %s", log_level_raw, argv[0]);
      exit(1);
    }
    config.log_level_state_machine = *log_level;
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.log_level_state_machine' in config filet at %s; using default "
      "value",
      argv[0]);
    config.log_level_state_machine = config.log_level;
  }
  // Telemetry log level
  if (config_object.HasMember("log_level_telemetry")) {
    const auto log_level_raw = config_object["log_level_telemetry"].GetInt();
    const auto log_level     = Logger::levelFromInt(log_level_raw);
    if (!log_level) {
      kInitialisationErrorLogger.error(
        "unknown value for system.log_level (%d) in config file at %s", log_level_raw, argv[0]);
      exit(1);
    }
    config.log_level_telemetry = *log_level;
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.log_level_telemetry' in config filet at %s; using default "
      "value",
      argv[0]);
    config.log_level_telemetry = config.log_level;
  }
  // Use fake trajectory?
  if (config_object.HasMember("use_fake_trajectory")) {
    config.use_fake_trajectory = config_object["use_fake_trajectory"].GetBool();
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.use_fake_trajectory' in config filet at %s; using default "
      "value",
      argv[0]);
    config.use_fake_trajectory = false;
  }
  // Use fake batteries?
  if (config_object.HasMember("use_fake_batteries")) {
    config.use_fake_batteries = config_object["use_fake_batteries"].GetBool();
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.use_fake_batteries' in config filet at %s; using default "
      "value",
      argv[0]);
    config.use_fake_batteries = false;
  }
  // Use fake batteries with fail?
  if (config_object.HasMember("use_fake_batteries_fail")) {
    config.use_fake_batteries_fail = config_object["use_fake_batteries_fail"].GetBool();
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.use_fake_batteries_fail' in config filet at %s; using default "
      "value",
      argv[0]);
    config.use_fake_batteries_fail = false;
  }
  // Use fake temperature?
  if (config_object.HasMember("use_fake_temperature")) {
    config.use_fake_temperature = config_object["use_fake_temperature"].GetBool();
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.use_fake_temperature' in config filet at %s; using default "
      "value",
      argv[0]);
    config.use_fake_temperature = false;
  }
  // Use fake temperature with fail?
  if (config_object.HasMember("use_fake_temperature_fail")) {
    config.use_fake_temperature_fail = config_object["use_fake_temperature_fail"].GetBool();
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.use_fake_temperature_fail' in config filet at %s; using "
      "default "
      "value",
      argv[0]);
    config.use_fake_temperature_fail = false;
  }
  // Use fake brakes?
  if (config_object.HasMember("use_fake_brakes")) {
    config.use_fake_brakes = config_object["use_fake_brakes"].GetBool();
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.use_fake_brakes' in config filet at %s; using default "
      "value",
      argv[0]);
    config.use_fake_brakes = false;
  }
  // Use fake controller?
  if (config_object.HasMember("use_fake_controller")) {
    config.use_fake_controller = config_object["use_fake_controller"].GetBool();
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.use_fake_controller' in config filet at %s; using default "
      "value",
      argv[0]);
    config.use_fake_controller = false;
  }
  // Use fake high _power?
  if (config_object.HasMember("use_fake_high_power")) {
    config.use_fake_high_power = config_object["use_fake_high_power"].GetBool();
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.use_fake_high_power' in config filet at %s; using default "
      "value",
      argv[0]);
    config.use_fake_high_power = false;
  }
  // Axis
  if (config_object.HasMember("axis")) {
    config.axis = static_cast<std::uint8_t>(config_object["axis"].GetUint());
  } else {
    kInitialisationErrorLogger.info(
      "could not find field 'system.axis' in config file at %s; using default value", argv[0]);
    config.axis = 0;
  }
  // We always re-construct the system when parseArgs is called successfully!
  // This makes testing easier because we can "restart" but one must make sure to
  // avoid using any outdated references.
  system_ = std::make_unique<System>(config);
}

std::uint64_t System::newRunId()
{
  static std::default_random_engine random_engine;
  static std::uniform_int_distribution<uint64_t> distribution(
    std::numeric_limits<std::uint64_t>::min(), std::numeric_limits<std::uint64_t>::max());
  return distribution(random_engine);
}

System &System::getSystem()
{
  if (system_) { return *system_; }
  kInitialisationErrorLogger.error(
    "somebody tried to access System before initialisation, aborting");
  exit(1);
}

Logger &System::getLogger()
{
  if (!system_) {
    kInitialisationErrorLogger.error(
      "somebody tried to access the system logger before initialisation, aborting");
    exit(1);
  }
  return system_->log_;
}

void System::interruptHandler(int)
{
  if (system_) { system_->stop(); }
  kInitialisationErrorLogger.info("interrupt signal received, exiting gracefully");
  exit(0);
}

void System::segfaultHandler(int)
{
  if (system_) { system_->stop(); }
  kInitialisationErrorLogger.error("forced termination detected (segfault?)");
  exit(1);
}

bool System::setExitFunction()
{
  static bool signal_set = false;
  if (signal_set) { return true; }

  // nominal termination
  std::signal(SIGINT, &interruptHandler);

  // forced termination
  std::signal(SIGSEGV, &segfaultHandler);
  std::signal(SIGABRT, &segfaultHandler);
  std::signal(SIGFPE, &segfaultHandler);
  std::signal(SIGILL, &segfaultHandler);
  std::signal(SIGTERM, &segfaultHandler);

  signal_set = true;
  return true;
}

bool System::isRunning()
{
  utils::concurrent::ScopedLock scoped_lock(&lock_);
  return running_;
}

void System::stop()
{
  utils::concurrent::ScopedLock scoped_lock(&lock_);
  running_ = false;
}

}  // namespace hyped::utils
