#include "recvloop.hpp"

#include <string>

namespace hyped {

using data::ModuleStatus;

namespace telemetry {

RecvLoop::RecvLoop(Logger &log, data::Data &data, Main *main_pointer)
    : Thread{log},
      main_ref_{*main_pointer},
      data_{data}
{
  log_.DBG("Telemetry", "Telemetry RecvLoop thread object created");
}

void RecvLoop::run()
{
  log_.DBG("Telemetry", "Telemetry RecvLoop thread started");

  data::Telemetry telem_data = data_.getTelemetryData();
  std::string message;

  while (true) {
    try {
      message = main_ref_.client_.receiveData();
    } catch (std::exception &e) {
      log_.ERR("Telemetry", "%s", e.what());

      telem_data.module_status = ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telem_data);

      break;
    }

    if (message == "ACK") {
      log_.INFO("Telemetry", "FROM SERVER: ACK");
    } else if (message == "STOP") {
      log_.INFO("Telemetry", "FROM SERVER: STOP");
      telem_data.emergency_stop_command = true;
    } else if (message == "CALIBRATE") {
      log_.INFO("Telemetry", "FROM SERVER: CALIBRATE");
      telem_data.calibrate_command = true;
    } else if (message == "LAUNCH") {
      log_.INFO("Telemetry", "FROM SERVER: LAUNCH");
      telem_data.launch_command = true;
    } else if (message == "SHUTDOWN") {
      log_.INFO("Telemetry", "FROM SERVER: SHUTDOWN");
      telem_data.shutdown_command = true;
    } else if (message == "SERVER_PROPULSION_GO") {
      log_.INFO("Telemetry", "FROM SERVER: SERVICE_PROPULSION_GO");
      telem_data.service_propulsion_go = true;
    } else if (message == "SERVER_PROPULSION_STOP") {
      log_.INFO("Telemetry", "FROM SERVER: SERVICE_PROPULSION_STOP");
      telem_data.service_propulsion_go = false;
    } else if (message == "NOMINAL_BRAKING") {
      log_.INFO("Telemetry", "FROM SERVER: NOMINAL_BRAKING");
      telem_data.nominal_braking_command = true;
    } else if (message == "NOMINAL_RETRACT") {
      log_.INFO("Telemetry", "FROM SERVER: NOMINAL_RETRACT");
      telem_data.nominal_braking_command = false;
    } else {
      log_.ERR("Telemetry", "Unrecognized input from server, ENTERING CRITICAL FAILURE");
      telem_data.module_status = ModuleStatus::kCriticalFailure;
    }

    data_.setTelemetryData(telem_data);
  }

  log_.DBG("Telemetry", "Exiting Telemetry RecvLoop thread");
}

}  // namespace telemetry
}  // namespace hyped
