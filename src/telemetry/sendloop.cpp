#include "sendloop.hpp"
#include "writer.hpp"
#include "data/data.hpp"

#include <string>

namespace hyped {
namespace telemetry {

SendLoop::SendLoop(utils::Logger &log, data::Data &data, Main &main_ref)
    : utils::concurrent::Thread{log},
      main_ref_{main_ref},
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

    data::Telemetry telemetry_data = data_.getTelemetryData();

    if (!main_ref_.client_.sendData(writer.getString())) {
      log_.ERR("Telemetry", "Error sending message");
      telemetry_data.module_status   = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telemetry_data);

      break;
    }

    // Increment num packages sent
    telemetry_data.num_packages_sent += 1;
    data_.setTelemetryData(telemetry_data);

    utils::concurrent::Thread::sleep(100);
  }

  log_.DBG("Telemetry", "Exiting Telemetry SendLoop thread");
}

}  // namespace telemetry
}  // namespace hyped
