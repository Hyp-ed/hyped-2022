#include "sendloop.hpp"
#include "writer.hpp"

#include <string>

#include "data/data.hpp"

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

  uint16_t num_packs_sent = 0;

  while (true) {
    Writer writer(data_);

    writer.start();
    writer.packTime();
    writer.packID(num_packs_sent);
    writer.packCrucialData();
    writer.packStatusData();
    writer.packAdditionalData();
    writer.end();

    data::Telemetry telemetry_data = data_.getTelemetryData();

    if (!main_ref_.client_.sendData(writer.getString())) {
      log_.ERR("Telemetry", "Error sending message");
      telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telemetry_data);

      break;
    }

    num_packs_sent += 1;

    utils::concurrent::Thread::sleep(100);
  }

  log_.DBG("Telemetry", "Exiting Telemetry SendLoop thread");
}

}  // namespace telemetry
}  // namespace hyped
