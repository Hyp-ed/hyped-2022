#include "fake_can_endpoint.hpp"

#include <utils/system.hpp>

namespace hyped::propulsion {
FakeCanEndpoint::FakeCanEndpoint(utils::io::ICanProcessor &sender)
    : utils::concurrent::Thread(
      utils::Logger("FAKE-CAN-ENDPOINT", utils::System::getSystem().config_.log_level_sensors)),
      sender_(sender)
{
}

void FakeCanEndpoint::run()
{
  utils::concurrent::Thread::sleep(1000);
  utils::io::can::Frame frame;
  sender_.processNewData(frame);
}

}  // namespace hyped::propulsion
