#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include <utils/logger.hpp>
#include <utils/utils.hpp>

namespace hyped::utils::io {

class Gpio {
 public:
  static constexpr uint8_t kBankNum = 4;
  enum class Direction { kIn = 0, kOut = 1 };
  /**
   * @brief to be called on when logger is not initialized
   * will call on following constructor after initializing logger with getLogger()
   *
   * @param pin address on BBB
   * @param direction to write into file system of gpio pin
   */
  Gpio(uint32_t pin, Direction direction);
  /**
   * @brief overload constructor with logger initialized for debugging purposes
   *
   * @param pin address on BBB
   * @param direction to write into file system of gpio pin
   * @param log
   */
  Gpio(uint32_t pin, Direction direction, Logger &log);

  void set();      // set high
  void clear();    // set low
  uint8_t read();  // read pin value

  /**
   * @brief Block caller until value of gpio pin has changed
   * @return int8_t the new gpio value, -1 in case of an error
   */
  int8_t wait();

 private:
  static constexpr std::array<off_t, kBankNum> kBases
    = {0x44e07000, 0x4804c000, 0x481ac000, 0x481ae000};
  static constexpr uint32_t kMmapSize = 0x1000;
  static constexpr uint32_t kData     = 0x138;
  static constexpr uint32_t kClear    = 0x190;
  static constexpr uint32_t kSet      = 0x194;
  Gpio()                              = delete;

  // GPIO system configuration
  /**
   * @brief Fill in base_mapping_ with pointers to mmap-ed /dev/mem
   * to 4 GPIO banks/ports.
   */

  /**
   * @brief Remaps each address for GPIO pins from kBankNum array.
   * Uses mmap, which creates a new mapping in the virtual address space
   * of the calling process based on a starting address and argument length.
   *
   * No logger initialized
   */
  static void initialise();

  /**
   * @brief Releases exported GPIO pins and writes value to buffer
   * of addresses of those pins to uninitialise them
   *
   * Double check this!
   *
   */
  static void uninitialise();

  /**
   * @brief boolean if is initialised
   *
   */
  static bool initialised_;

  /**
   * @brief arary of size kBankNum, used in initialize() and attatchGPIO()
   *
   */
  static void *base_mapping_[kBankNum];

  /**
   * @brief vector of currently used (exported) pins
   *
   */
  static std::vector<uint32_t> exported_pins;

  /**
   * @brief Tell kernel we are using this, set direction
   */
  void exportGPIO();

  /**
   * @brief Configure device register pointers
   */
  void attachGPIO();

  /**
   * @brief Configure fd_ for wait functionality in case of input GPIO
   */
  void setupWait();

  uint32_t pin_;
  Direction direction_;
  Logger &log_;

  volatile uint32_t *set_;    // set register
  volatile uint32_t *clear_;  // clear register
  volatile uint32_t *data_;   // data register
  uint32_t pin_mask_;         // mask for register access to this pin
  int fd_;                    // file pointer to /sys/class/gpio/gpioXX/value

  NO_COPY_ASSIGN(Gpio)
};

}  // namespace hyped::utils::io
