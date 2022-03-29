#pragma once

#include <cstdint>
#include <vector>

#include <utils/logger.hpp>
#include <utils/utils.hpp>

namespace hyped {
namespace utils {
// Forward declaration
class Logger;
namespace io {

namespace adc {
}  // namespace adc

class Adc {
 public:
  /**
   * @brief Construct a new ADC object when logger no initialised
   *
   * @param pin
   */
  explicit Adc(uint32_t pin);

  /**
   * @brief Construct a new ADC object with logger for debugging purposes
   *
   * @param pin
   * @param log
   */
  Adc(uint32_t pin, Logger &log);

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
