#include "sender.hpp"
#include "writer.hpp"

#include <string>

#include <data/data.hpp>

namespace hyped::telemetry {

Sender::Sender(data::Data &data, Client &client)
    : utils::concurrent::Thread(
      utils::Logger("SENDER", utils::System::getSystem().config_.log_level_telemetry)),
      sys_(utils::System::getSystem()),
      data_(data),
      client_(client)
{
  log_.debug("Telemetry Sender thread object created");
}

void Sender::run()
{
  log_.debug("Telemetry Sender thread started");

  int num_packages_sent = 0;

  while (sys_.isRunning()) {
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

    data::Telemetry telemetry_data = data_.getTelemetryData();
    if (!client_.sendData(writer.getString())) {
      telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
      data_.setTelemetryData(telemetry_data);
      break;
    }
    ++num_packages_sent;
    utils::concurrent::Thread::sleep(100);
  }

  log_.debug("Exiting Telemetry Sender thread");
}

}  // namespace hyped::telemetry
