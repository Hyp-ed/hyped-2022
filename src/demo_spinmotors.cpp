#include <stdlib.h>

#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "propulsion/controller.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::motor_control::Controller;

int32_t readVel(const char* filepath, Logger& log)
{
  FILE* fp;

  fp = fopen(filepath, "r");

  int32_t vel = 0;

  if (fp == NULL) {
  } else {
    char line[255];
    fgets(line, 255, fp);
    log.INFO("TEST", "line= %s",line);

    vel = std::atoi(line);
  }
  fclose(fp);
  return vel;
}

int main(int argc, char** argv) {
  System::parseArgs(argc, argv);
  Logger log = System::getLogger();

  Controller controller(log, 5);

  controller.registerController();
  controller.configure();

  int32_t vel = 0;
  controller.sendTargetVelocity(vel);
  controller.enterOperational();

  // Thread::sleep(1000);
  // controller.sendTargetVelocity(10);

  // Thread::sleep(100000);

  while(1) {
  vel = readVel("data/in/target_velocity_test.txt", log);
  log.INFO("TEST", "vel= %d", vel);
  controller.sendTargetVelocity(vel);
  Thread::sleep(1000);
  }
  
  // controller.autoAlignMotorPosition();

  // controller.sendTargetVelocity(50);

  // for (auto i = 0; i < 20; i++) {
  //   controller.updateActualVelocity();
  //   auto vel = controller.getVelocity();
  //   log.INFO("TEST", "Actual velocity: %d rpm", vel);
  //   Thread::sleep(1000);
  // }
  // controller.enterPreOperational();
}