#include "recvloop.hpp"

#include <string>

namespace hyped::telemetry {

ReceiveLoop::ReceiveLoop(utils::Logger &log, data::Data &data, Client &client)
    : utils::concurrent::Thread{log},
      client_{client},
      data_{data}
{
  log_.DBG("RECEIVE-LOOP", "constructed");
}

void ReceiveLoop::run()
{
  log_.DBG("RECEIVE-LOOP", "thread started");
  while (true) {
    auto telemetry_data = data_.getTelemetryData();
    std::string message;
    try {
      message = client_.receiveData();
    } catch (std::exception &e) {
      log_.ERR("RECEIVE-LOOP", "%s", e.what());
      telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telemetry_data);
      break;
    }
    if (message == "ACK") {
      log_.INFO("RECEIVE-LOOP", "FROM SERVER: ACK");
    } else if (message == "STOP") {
      log_.INFO("RECEIVE-LOOP", "FROM SERVER: STOP");
      telemetry_data.emergency_stop_command = true;
    } else if (message == "CALIBRATE") {
      log_.INFO("RECEIVE-LOOP", "FROM SERVER: CALIBRATE");
      telemetry_data.calibrate_command = true;
    } else if (message == "LAUNCH") {
      log_.INFO("RECEIVE-LOOP", "FROM SERVER: LAUNCH");
      telemetry_data.launch_command = true;
    } else if (message == "SHUTDOWN") {
      log_.INFO("RECEIVE-LOOP", "FROM SERVER: SHUTDOWN");
      telemetry_data.shutdown_command = true;
    } else if (message == "SERVER_PROPULSION_GO") {
      log_.INFO("RECEIVE-LOOP", "FROM SERVER: SERVICE_PROPULSION_GO");
      telemetry_data.service_propulsion_go = true;
    } else if (message == "SERVER_PROPULSION_STOP") {
      log_.INFO("RECEIVE-LOOP", "FROM SERVER: SERVICE_PROPULSION_STOP");
      telemetry_data.service_propulsion_go = false;
    } else if (message == "NOMINAL_BRAKING") {
      log_.INFO("RECEIVE-LOOP", "FROM SERVER: NOMINAL_BRAKING");
      telemetry_data.nominal_braking_command = true;
    } else if (message == "NOMINAL_RETRACT") {
      log_.INFO("RECEIVE-LOOP", "FROM SERVER: NOMINAL_RETRACT");
      telemetry_data.nominal_braking_command = false;
    } else {
      log_.ERR("RECEIVE-LOOP", "Unrecognized input from server, ENTERING CRITICAL FAILURE");
      telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
    }
    data_.setTelemetryData(telemetry_data);
  }
  log_.DBG("RECEIVE-LOOP", "Exiting Telemetry RecvLoop thread");
}

}  // namespace hyped::telemetry
