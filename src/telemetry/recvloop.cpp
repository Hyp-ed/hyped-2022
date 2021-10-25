#include "recvloop.hpp"

#include <string>

namespace hyped {

namespace telemetry {

RecvLoop::RecvLoop(utils::Logger &log, data::Data &data, utils::Client &client)
    : utils::concurrent::Thread{log},
      client_{client},
      data_{data}
{
  log_.DBG("Telemetry", "Telemetry RecvLoop thread object created");
}

void RecvLoop::run()
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
    switch (message){
      case "ACK":
        log_.INFO("Telemetry", "FROM SERVER: ACK");
        break;
      case "STOP":
        log_.INFO("Telemetry", "FROM SERVER: STOP");
        telemetry_data.emergency_stop_command = true;
        break;
      case "CALIBRATE":
        log_.INFO("Telemetry", "FROM SERVER: CALIBRATE");
        telemetry_data.calibrate_command = true;
        break;
      case "LAUNCH":
        log_.INFO("Telemetry", "FROM SERVER: LAUNCH");
        telemetry_data.launch_command = true;
        break;
      case "SHUTDOWN":
        log_.INFO("Telemetry", "FROM SERVER: SHUTDOWN");
        telemetry_data.shutdown_command = true;
        break;
      case "SERVER_PROPULSION_GO":
        log_.INFO("Telemetry", "FROM SERVER: SERVICE_PROPULSION_GO");
        telemetry_data.service_propulsion_go = true;
        break;
      case "SERVER_PROPULSION_STOP":
        log_.INFO("Telemetry", "FROM SERVER: SERVICE_PROPULSION_STOP");
        telemetry_data.service_propulsion_go = false;
        break;
      case "NOMINAL_BRAKING":
        log_.INFO("Telemetry", "FROM SERVER: NOMINAL_BRAKING");
        telemetry_data.nominal_braking_command = true;
        break;
      case "NOMINAL_RETRACT":
        log_.INFO("Telemetry", "FROM SERVER: NOMINAL_RETRACT");
        telemetry_data.nominal_braking_command = false;
        break;
      default:
        log_.ERR("Telemetry", "Unrecognized input from server, ENTERING CRITICAL FAILURE");
        telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
    }
    
    } 
    }

    data_.setTelemetryData(telemetry_data);
  }

  log_.DBG("Telemetry", "Exiting Telemetry RecvLoop thread");
}

}  // namespace telemetry
}  // namespace hyped
