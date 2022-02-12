#include <propulsion/can/can_sender.hpp>
#include <utils/io/can.hpp>
#include <utils/logger.hpp>

namespace hyped::propulsion {

class NucleoManager {
 public:
  /**
   * @brief Initializes the nucleo manager with the logger
   * */
  NucleoManager(utils::Logger &log);
  /**
   * @brief Send the target frequency to the nucleo board.
   * @param target_frequency in Hz
   */
  void sendNucleoFrequency(const uint32_t target_frequency);

 private:
  utils::Logger &log_;
  utils::io::can::Frame nucleo_message_;
  CanSender transceiver_;
};
}  // namespace hyped::propulsion
