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

  uint32_t num_packages_sent = 0;

  while (true) {
    Writer writer;

    writer.start();
    writer.packTime();
    writer.packId(num_packages_sent);
    writer.packTelemetryData();
    writer.packSensorsData();
    writer.packMotorData();
    writer.packStateMachineData();
    writer.packNavigationData();
    writer.end();

    if (!client_.sendData(writer.getString())) {
      data::Telemetry telemetry_data = data_.getTelemetryData();
      log_.ERR("Telemetry", "Error sending message");
      telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telemetry_data);

      // Need to use writer.getString() and send the string.

      break;
    }
    ++num_packages_sent;
    utils::concurrent::Thread::sleep(100);
  }

  log_.DBG("Telemetry", "Exiting Telemetry Sender thread");
}

}  // namespace hyped::telemetry
