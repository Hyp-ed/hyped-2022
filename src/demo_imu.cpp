#include "sensors/imu.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;
using hyped::sensors::Imu;


int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, -1);

  bool is_fifo = false;     // replace with config parse

  Imu imu0(log, 20, is_fifo);

  ImuData data0;
  for (int i = 0; i < 50; i++) {
    imu0.getData(&data0);
    if (is_fifo) {
      for (int i = 0; i < ImuData::kFifo; i++) {
        log.INFO("TEST-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2",
                  0,
                  data0.fifo[i][0],
                  data0.fifo[i][1],
                  data0.fifo[i][2]);
      }
    } else {
      log.INFO("TEST-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2",
                0,
                data0.acc[0],
                data0.acc[1],
                data0.acc[2]);
      Thread::sleep(100);
    }
    Thread::sleep(100);
  }
 	return 0;
}
