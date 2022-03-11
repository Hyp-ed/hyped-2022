#include <propulsion/can/can_sender.hpp>
#include <propulsion/can/sender_interface.hpp>
#include <utils/io/can.hpp>
#include <utils/logger.hpp>

namespace hyped::propulsion {

class NucleoManager {
 public:
  /**
   * @brief Initializes the nucleo manager
   * */
  NucleoManager();
  /**
   * @brief Send the target frequency to the nucleo board.
   * @param target_frequency in Hz
   */
  void sendNucleoFrequency(const uint32_t target_frequency);

 private:
  utils::Logger log_;
  utils::io::can::Frame can_frame_;
  CanSender sender_;
};
}  // namespace hyped::propulsion
