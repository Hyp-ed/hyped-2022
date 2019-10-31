#include "utils/io/spi.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"

using hyped::utils::io::SPI;
using hyped::utils::Logger;

int main(int argc, char* argv[]) {
  hyped::utils::System::parseArgs(argc, argv);
  Logger& log = hyped::utils::System::getLogger();

  SPI& spi = (SPI::getInstance());

  uint8_t data_out[] = {1,2,3,4};
  uint8_t data_in[]  = {0,0,0,0};


  log.INFO("Before IN", "1 = %d, 2 = %d, 3 = %d, 4 = %d", data_in[0], 
   data_in[1], data_in[2], data_in[3]);

  log.INFO("OUT", "1 = %d, 2 = %d, 3 = %d, 4 = %d", data_out[0], 
  data_out[1], data_out[2], data_out[3]);

  spi.transfer(data_out, data_in, 4);

  

  log.INFO("IN", "1 = %d, 2 = %d, 3 = %d, 4 = %d", data_in[0], 
  data_in[1], data_in[2], data_in[3]);
}