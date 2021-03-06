#include "main.hpp"
#include "receiver.hpp"
#include "sender.hpp"

#include <utils/system.hpp>

namespace hyped::telemetry {

Main::Main()
    : utils::concurrent::Thread(
      utils::Logger("TELEMETRY", utils::System::getSystem().config_.log_level_telemetry)),
      data_(data::Data::getInstance())
{
  const auto &system_config = utils::System::getSystem().config_;
  auto client_optional      = Client::fromFile(system_config.client_config_path);
  if (!client_optional) {
    log_.error("failed to parse client config in config file at %s",
               system_config.client_config_path.c_str());
    utils::System::getSystem().stop();
  }
  client_ = std::make_unique<Client>(*client_optional);
}

void Main::run()
{
  auto telemetry_data = data_.getTelemetryData();
  log_.debug("Telemetry Main thread started");
  try {
    client_->connect();
  } catch (std::exception &e) {
    log_.error(e.what());
    log_.error("Exiting Telemetry Main thread (due to error connecting)");

    telemetry_data.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setTelemetryData(telemetry_data);

    return;
  }
  telemetry_data.module_status = data::ModuleStatus::kReady;
  data_.setTelemetryData(telemetry_data);

  Sender sender(data_, *client_);
  Receiver receiver(data_, *client_);
  sender.start();
  receiver.start();
  sender.join();
  receiver.join();

  log_.debug("Exiting Telemetry Main thread");
}

}  // namespace hyped::telemetry
