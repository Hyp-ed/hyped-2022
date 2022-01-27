#include "receiver.hpp"

#include <string>

#include <utils/system.hpp>

namespace hyped::telemetry {

Receiver::Receiver(data::Data &data, Client &client)
    : utils::concurrent::Thread(
      utils::Logger("RECEIVER", utils::System::getSystem().config_.log_level_telemetry)),
      client_{client},
      data_{data}
{
  log_.debug("constructed");
}

void Receiver::run()
{
  log_.debug("thread started");
  while (true) {
    auto telemetry_data = data_.getTelemetryData();
    std::string message;
    try {
      message = client_.receiveData();
    } catch (std::exception &e) {
      log_.error("%s", e.what());
      telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telemetry_data);
      break;
    }
    if (message == "ACK") {
      log_.info("FROM SERVER: ACK");
    } else if (message == "STOP") {
      log_.info("FROM SERVER: STOP");
      telemetry_data.emergency_stop_command = true;
    } else if (message == "CALIBRATE") {
      log_.info("FROM SERVER: CALIBRATE");
      telemetry_data.calibrate_command = true;
    } else if (message == "LAUNCH") {
      log_.info("FROM SERVER: LAUNCH");
      telemetry_data.launch_command = true;
    } else if (message == "SHUTDOWN") {
      log_.info("FROM SERVER: SHUTDOWN");
      telemetry_data.shutdown_command = true;
    } else if (message == "SERVER_PROPULSION_GO") {
      log_.info("FROM SERVER: SERVICE_PROPULSION_GO");
      telemetry_data.service_propulsion_go = true;
    } else if (message == "SERVER_PROPULSION_STOP") {
      log_.info("FROM SERVER: SERVICE_PROPULSION_STOP");
      telemetry_data.service_propulsion_go = false;
    } else if (message == "NOMINAL_BRAKING") {
      log_.info("FROM SERVER: NOMINAL_BRAKING");
      telemetry_data.nominal_braking_command = true;
    } else if (message == "NOMINAL_RETRACT") {
      log_.info("FROM SERVER: NOMINAL_RETRACT");
      telemetry_data.nominal_braking_command = false;
    } else {
      log_.error("Unrecognized input from server, ENTERING CRITICAL FAILURE");
      telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
    }
    data_.setTelemetryData(telemetry_data);
  }
  log_.debug("Exiting Telemetry RecvLoop thread");
}

}  // namespace hyped::telemetry
