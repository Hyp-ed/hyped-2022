#include "main.hpp"
#include "receiver.hpp"
#include "sender.hpp"

#include <utils/system.hpp>

namespace hyped {

namespace telemetry {

Main::Main(const uint8_t id, utils::Logger &log)
    : utils::concurrent::Thread{id, log},
      data_{data::Data::getInstance()},
      client_{log}
{
  log_.DBG("Telemetry", "Telemetry Main thread object created");
}

void Main::run()
{
  // check if telemetry is disabled
  const auto &sys                = utils::System::getSystem();
  data::Telemetry telemetry_data = data_.getTelemetryData();

  if (sys.telemetry_off) {
    log_.DBG("Telemetry", "Telemetry is disabled");
    log_.DBG("Telemetry", "Exiting Telemetry Main thread");
    telemetry_data.module_status = data::ModuleStatus::kReady;
    data_.setTelemetryData(telemetry_data);
    return;
  }

  log_.DBG("Telemetry", "Telemetry Main thread started");

  try {
    client_.connect();
  } catch (std::exception &e) {
    log_.ERR("Telemetry", e.what());
    log_.ERR("Telemetry", "Exiting Telemetry Main thread (due to error connecting)");

    telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setTelemetryData(telemetry_data);

    return;
  }

  telemetry_data.module_status = data::ModuleStatus::kReady;
  data_.setTelemetryData(telemetry_data);

  Sender send_loop_thread{log_, data_, this->client_};
  Receiver receive_loop_thread{log_, data_, this->client_};
  send_loop_thread.start();
  receive_loop_thread.start();
  send_loop_thread.join();
  receive_loop_thread.join();

  log_.DBG("Telemetry", "Exiting Telemetry Main thread");
}

}  // namespace telemetry
}  // namespace hyped
