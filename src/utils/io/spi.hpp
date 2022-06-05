#pragma once

#include <utils/logger.hpp>
#include <utils/utils.hpp>

namespace hyped {
namespace utils {
namespace io {

// forward declaration
struct SPI_HW;
struct SPI_CH;

class Spi {
 public:
  static Spi &getInstance();

  enum class Clock { k1MHz, k4MHz, k16MHz, k20MHz };

  void setClock(Clock clk);

  /**
   * @brief simultaneous write and read. Write and read buffer should have the same length
   * @param tx  - pointer to head of write buffer
   * @param rx  - pointer to head of read  buffer
   * @param len - number of BYTES in each buffer
   */
  void transfer(uint8_t *tx, uint8_t *rx, uint16_t len);

  /**
   * @brief Get data from sensor, starting at some address.
   * @param addr  - register from which the reading should start
   * @param rx    - pointer to head of read buffer
   * @param len   - number of BYTES to be read, i.e. size of the read buffer
   */
  void read(uint8_t addr, uint8_t *rx, uint16_t len);

  /**
   * @brief Write data to sensor, starting at some address.
   * @param addr  - register from which writing to starts
   * @param tx    - pointer to head of write buffer
   * @param len   - number of BYTES to be written, i.e. size of the write buffer
   */
  void write(uint8_t addr, uint8_t *tx, uint16_t len);

 private:
  explicit Spi(Logger &log);
  ~Spi();
  /**
   * @brief Fill in base_mapping_ with pointers to mmap-ed /dev/spidev1.0
   * to 2 SPI banks/ports.
   */
  bool initialise();

 private:
  int spi_fd_;
  SPI_HW *hw_;
  SPI_CH *ch_;
  Logger &log_;

  NO_COPY_ASSIGN(Spi)
};

}  // namespace io
}  // namespace utils
}  // namespace hyped
