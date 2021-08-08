#pragma once

#include <cstdint>
#include <utils/logger.hpp>
#include <utils/utils.hpp>
#include <vector>

namespace hyped {
namespace utils {
// Forward declaration
class Logger;
namespace io {

namespace adc {
}  // namespace adc

class ADC {
 public:
  /**
   * @brief Construct a new ADC object when logger no initialised
   *
   * @param pin
   */
  explicit ADC(uint32_t pin);

  /**
   * @brief Construct a new ADC object with logger for debugging purposes
   *
   * @param pin
   * @param log
   */
  ADC(uint32_t pin, Logger &log);

  /**
   * @brief reads AIN value from file system
   *
   * @return uint16_t return two bytes for [0,4095] range
   */
  uint16_t read();

 private:
  uint32_t pin_;
  Logger &log_;
  int fd_;
};
}  // namespace io
}  // namespace utils
}  // namespace hyped
