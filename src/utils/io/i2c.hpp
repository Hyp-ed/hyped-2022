#include <cstdint>

#include <utils/logger.hpp>

namespace hyped::utils::io {

class I2c {
 public:
  I2c(const uint8_t busAddr);
  ~I2c();

  void readData(const uint32_t addr, uint8_t *data, const size_t len);
  void writeData(const uint32_t addr, uint8_t *data, const size_t len);

 private:
  void setSensorAddress(uint32_t addr);

 private:
  utils::Logger log_;
  int fd_;
  uint32_t sensor_addr_;
};

}  // namespace hyped::utils::io