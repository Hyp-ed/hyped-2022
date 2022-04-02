#include <cstdint>

#include <utils/logger.hpp>

namespace hyped::utils::io {

class I2c {
 public:
  I2c(const uint8_t bus_address);
  ~I2c();

  int readData(const uint32_t address, uint8_t *data, const size_t len);
  int writeData(const uint32_t address, uint8_t *data, const size_t len);

 private:
  void setSensorAddress(uint32_t address);

 private:
  utils::Logger log_;
  int fd_;
  uint32_t sensor_address_;
};

}  // namespace hyped::utils::io