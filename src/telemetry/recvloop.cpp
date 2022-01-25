#include "recvloop.hpp"

#include <string>

namespace hyped {

using data::ModuleStatus;

namespace telemetry {

RecvLoop::RecvLoop(utils::Logger &log, data::Data &data, Main *main_pointer)
    : Thread{log},
      main_ref_{*main_pointer},
      data_{data}
{
  log_.debug("Telemetry RecvLoop thread object created");
}

void RecvLoop::run()
{
  log_.debug("Telemetry RecvLoop thread started");

  data::Telemetry telem_data_struct = data_.getTelemetryData();
  std::string message;

  while (true) {
    try {
      message = main_ref_.client_->receiveData();
    } catch (std::exception &e) {
      log_.error("%s", e.what());

      telem_data_struct.module_status = ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telem_data_struct);

      break;
    }

    if (message == "ACK") {
      log_.info("FROM SERVER: ACK");
    } else if (message == "STOP") {
      log_.info("FROM SERVER: STOP");
      telem_data_struct.emergency_stop_command = true;
    } else if (message == "CALIBRATE") {
      log_.info("FROM SERVER: CALIBRATE");
      telem_data_struct.calibrate_command = true;
    } else if (message == "LAUNCH") {
      log_.info("FROM SERVER: LAUNCH");
      telem_data_struct.launch_command = true;
    } else if (message == "SHUTDOWN") {
      log_.info("FROM SERVER: SHUTDOWN");
      telem_data_struct.shutdown_command = true;
    } else if (message == "SERVER_PROPULSION_GO") {
      log_.info("FROM SERVER: SERVICE_PROPULSION_GO");
      telem_data_struct.service_propulsion_go = true;
    } else if (message == "SERVER_PROPULSION_STOP") {
      log_.info("FROM SERVER: SERVICE_PROPULSION_STOP");
      telem_data_struct.service_propulsion_go = false;
    } else if (message == "NOMINAL_BRAKING") {
      log_.info("FROM SERVER: NOMINAL_BRAKING");
      telem_data_struct.nominal_braking_command = true;
    } else if (message == "NOMINAL_RETRACT") {
      log_.info("FROM SERVER: NOMINAL_RETRACT");
      telem_data_struct.nominal_braking_command = false;
    } else {
      log_.error("Unrecognized input from server, ENTERING CRITICAL FAILURE");
      telem_data_struct.module_status = ModuleStatus::kCriticalFailure;
    }

    data_.setTelemetryData(telem_data_struct);
  }

  log_.debug("Exiting Telemetry RecvLoop thread");
}

}  // namespace telemetry
}  // namespace hyped
