#include "recvloop.hpp"

#include <string>

namespace hyped {

namespace telemetry {

RecieveLoop::RecieveLoop(utils::Logger &log, data::Data &data, Client &client)
    : utils::concurrent::Thread{log},
      client_{client},
      data_{data}
{
  log_.DBG("Telemetry", "Telemetry RecvLoop thread object created");
}

void RecieveLoop::run()
{
  while (true) {
    log_.DBG("Telemetry", "Telemetry RecvLoop thread started");

    data::Telemetry telemetry_data = data_.getTelemetryData();
    std::string message;
    try {
      message = client_.receiveData();
    } catch (std::exception &e) {
      log_.ERR("Telemetry", "%s", e.what());

      telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telemetry_data);

      break;
    }
    if (message == "ACK") {
      log_.INFO("Telemetry", "FROM SERVER: ACK");
    } else if (message == "STOP") {
      log_.INFO("Telemetry", "FROM SERVER: STOP");
      telem_data_struct.emergency_stop_command = true;
    } else if (message == "CALIBRATE") {
      log_.INFO("Telemetry", "FROM SERVER: CALIBRATE");
      telem_data_struct.calibrate_command = true;
    } else if (message == "LAUNCH") {
      log_.INFO("Telemetry", "FROM SERVER: LAUNCH");
      telem_data_struct.launch_command = true;
    } else if (message == "SHUTDOWN") {
      log_.INFO("Telemetry", "FROM SERVER: SHUTDOWN");
      telem_data_struct.shutdown_command = true;
    } else if (message == "SERVER_PROPULSION_GO") {
      log_.INFO("Telemetry", "FROM SERVER: SERVICE_PROPULSION_GO");
      telem_data_struct.service_propulsion_go = true;
    } else if (message == "SERVER_PROPULSION_STOP") {
      log_.INFO("Telemetry", "FROM SERVER: SERVICE_PROPULSION_STOP");
      telem_data_struct.service_propulsion_go = false;
    } else if (message == "NOMINAL_BRAKING") {
      log_.INFO("Telemetry", "FROM SERVER: NOMINAL_BRAKING");
      telem_data_struct.nominal_braking_command = true;
    } else if (message == "NOMINAL_RETRACT") {
      log_.INFO("Telemetry", "FROM SERVER: NOMINAL_RETRACT");
      telem_data_struct.nominal_braking_command = false;
    } else {
      log_.ERR("Telemetry", "Unrecognized input from server, ENTERING CRITICAL FAILURE");
      telem_data_struct.module_status = ModuleStatus::kCriticalFailure;
    }
  }
}

data_.setTelemetryData(telemetry_data);
}  // namespace telemetry

log_.DBG("Telemetry", "Exiting Telemetry RecvLoop thread");
}  // namespace hyped



