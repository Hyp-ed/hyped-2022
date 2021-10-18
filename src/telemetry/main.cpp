#include "main.hpp"
#include "recvloop.hpp"
#include "sendloop.hpp"

#include <utils/system.hpp>

namespace hyped {

namespace telemetry {

Main::Main(uint8_t id, Logger &log)
    : Thread{id, log},
      data_{data::Data::getInstance()},
      client_{log}
{
  log_.DBG("Telemetry", "Telemetry Main thread object created");
}

void Main::run()
{
  // check if telemetry is disabled
  hyped::utils::System &sys             = hyped::utils::System::getSystem();
  data::Telemetry telemetry_data_struct = data_.getTelemetryData();

  if (sys.telemetry_off) {
    log_.DBG("Telemetry", "Telemetry is disabled");
    log_.DBG("Telemetry", "Exiting Telemetry Main thread");
    telemetry_data_struct.module_status = data::ModuleStatus::kReady;
    data_.setTelemetryData(telemetry_data_struct);
    return;
  }

  log_.DBG("Telemetry", "Telemetry Main thread started");

  try {
    client_.connect();
  } catch (std::exception &e) {
    log_.ERR("Telemetry", e.what());
    log_.ERR("Telemetry", "Exiting Telemetry Main thread (due to error connecting)");

    telemetry_data_struct.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setTelemetryData(telemetry_data_struct);

    return;
  }

  telemetry_data_struct.module_status = data::ModuleStatus::kReady;
  data_.setTelemetryData(telemetry_data_struct);

  SendLoop sendloop_thread{log_, data_, this};
  RecvLoop recvloop_thread{log_, data_, this};
  sendloop_thread.start();
  recvloop_thread.start();
  sendloop_thread.join();
  recvloop_thread.join();

  log_.DBG("Telemetry", "Exiting Telemetry Main thread");
}

}  // namespace telemetry
}  // namespace hyped
