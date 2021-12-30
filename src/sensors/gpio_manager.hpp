#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/config.hpp>
#include <utils/io/gpio.hpp>
#include <utils/system.hpp>

namespace hyped::sensors {
/**
 * Please see wiki for high-level description of this shared class for our system.
 * For new hardware devices, you must add: (vector of) GPIO*, previous_<module>_status_, and
 * appropriate hardware actuation functions. Use switch statements in run() to check status/state
 * and call appropriate functions. You should not need to other files since switch statements should
 * check data directly from data.hpp.g
 */

class GpioManager : public utils::concurrent::Thread {
 public:
  explicit GpioManager(utils::Logger &log);
  void run() override;

 private:
  void clearHighPower();

  void setHighPower();

  utils::System &sys_;
  data::Data &data_;
  /**
   * @brief master switch to keep pod on, signal held high at startup
   */
  std::unique_ptr<utils::io::GPIO> master_;

  /**
   * @brief SSR switches for HP battery packs
   *
   */
  std::vector<std::unique_ptr<utils::io::GPIO>> high_power_ssr_;

  /**
   * @brief stores the previous state when switch statement checks state machine
   *        conditional statement prevents repetitive actuation
   */
  data::State previous_state_;
};

}  // namespace hyped::sensors
