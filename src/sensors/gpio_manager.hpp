#pragma once

#include <cstdint>
#include <data/data.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/config.hpp>
#include <utils/io/gpio.hpp>
#include <utils/system.hpp>
#include <vector>

namespace hyped {

using data::Data;
using utils::Logger;
using utils::concurrent::Thread;
using utils::io::GPIO;

namespace sensors {
/**
 * Please see wiki for high-level description of this shared class for our system.
 * For new hardware devices, you must add: (vector of) GPIO*, previous_<module>_status_, and
 * appropriate hardware actuation functions. Use switch statements in run() to check status/state
 * and call appropriate functions. You should not need to other files since switch statements should
 * check data directly from data.hpp.g
 */

class GpioManager : public Thread {
 public:
  explicit GpioManager(Logger &log);
  void run() override;

 private:
  void clearHP();

  void setHP();

  utils::System &sys_;
  data::Data &data_;
  /**
   * @brief master switch to keep pod on, signal held high at startup
   */
  GPIO *master_;

  /**
   * @brief SSR switches for HP battery packs
   *
   */
  std::vector<GPIO *> hp_ssr_;

  /**
   * @brief stores the previous state when switch statement checks state machine
   *        conditional statement prevents repetitive actuation
   */
  data::State previous_state_;

  /**
   * @brief stores the previous state when switch statement checks state machine
   *        conditional statement prevents repetitive actuation
   */
  data::ModuleStatus previous_battery_status_;
};

}  // namespace sensors
}  // namespace hyped
