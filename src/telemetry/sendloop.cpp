#include "sendloop.hpp"
#include "writer.hpp"

#include <string>

namespace hyped {
namespace telemetry {

SendLoop::SendLoop(Logger &log, data::Data &data, Main *main_pointer)
    : Thread{log},
      main_ref_{*main_pointer},
      data_{data}
{
  log_.DBG("Telemetry", "Telemetry SendLoop thread object created");
}

void SendLoop::run()
{
  log_.DBG("Telemetry", "Telemetry SendLoop thread started");

  while (true) {
    Writer writer(data_);

    writer.start();
    writer.packTime();
    writer.packCrucialData();
    writer.packStatusData();
    writer.packAdditionalData();
    writer.end();

    if (!main_ref_.client_.sendData(writer.getString())) {
      log_.ERR("Telemetry", "Error sending message");
      data::Telemetry telemetry_data = data_.getTelemetryData();
      telemetry_data.module_status   = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telemetry_data);

      break;
    }

    Thread::sleep(100);
  }

  log_.DBG("Telemetry", "Exiting Telemetry SendLoop thread");
}

}  // namespace telemetry
}  // namespace hyped
