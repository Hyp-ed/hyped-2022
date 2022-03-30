#include <cstdint>

#include <utils/logger.hpp>

namespace hyped::utils::io {

class I2C {
 public:
  I2C();
  ~I2C();

  void readData(uint32_t addr, uint8_t *data, uint8_t len);
  void writeData(uint32_t addr, uint8_t *data, uint8_t len);

 private:
  void setSensorAddress(uint32_t addr);

 private:
  utils::Logger log_;
  int fd_;
  uint32_t sensor_addr_;
};

}  // namespace hyped::utils::io