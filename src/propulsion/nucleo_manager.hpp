#include <propulsion/can/can_sender.hpp>
#include <utils/io/can.hpp>
#include <utils/logger.hpp>

namespace hyped::propulsion {

class StateProcessor {
 public:
  /**
   * @brief Send the target frequency to the nucleo board.
   * @param target_frequency in Hz
   */
  void sendNucleoFrequency(const uint32_t target_frequency);

 private:
  utils::io::can::Frame nucleo_message_;
  CanSender sender_;
};
}  // namespace hyped::propulsion
