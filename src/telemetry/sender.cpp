#include "sender.hpp"
#include "writer.hpp"

#include <string>

#include "data/data.hpp"

namespace hyped::telemetry {

Sender::Sender(utils::Logger &log, data::Data &data, Client &client)
    : utils::concurrent::Thread{log},
      client_{client},
      data_{data}
{
  log_.DBG("Telemetry", "Telemetry Sender thread object created");
}

void Sender::run()
{
  log_.DBG("Telemetry", "Telemetry Sender thread started");

  uint16_t num_packages_sent = 0;

  while (true) {
    Writer writer(data_);
    writer.start();
    writer.packTime();
    writer.packId(num_packages_sent);
    writer.packCrucialData();
    writer.packStatusData();
    writer.packAdditionalData();
    writer.end();
    data::Telemetry telemetry_data = data_.getTelemetryData();
    if (!client_.sendData(writer.getString())) {
      log_.ERR("Telemetry", "Error sending message");
      telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telemetry_data);

      break;
    }
    ++num_packages_sent;
    utils::concurrent::Thread::sleep(100);
  }

  log_.DBG("Telemetry", "Exiting Telemetry Sender thread");
}

}  // namespace hyped::telemetry
