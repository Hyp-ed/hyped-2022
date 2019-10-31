#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "utils/concurrent/thread.hpp"
#include "propulsion/controller.hpp"
#include "propulsion/can/can_sender.hpp"
#include "utils/io/can.hpp"
#include "utils/concurrent/thread.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::motor_control::CanSender;
using hyped::motor_control::Controller;
using hyped::utils::io::can::Frame;
using hyped::utils::concurrent::Thread;


int main(int argc, char** argv) {
  System::parseArgs(argc, argv);
  Logger log = System::getLogger();

  int node_id = 2;
  int kNmtOperational = 0x01;
  uint32_t kNmtReceive = 0x000;

  Frame nmt_message;
  nmt_message.id = kNmtReceive;
  nmt_message.extended = false;
  nmt_message.len = 2;
  nmt_message.data[0] = kNmtOperational;
  nmt_message.data[1] = node_id;

  CanSender sender(log, node_id);
  Controller controller(log, node_id);

  controller.registerController();
  // controller.enterOperational();

  // Send nmt message to transition from not ready to switch on to
  // switch on disabled

  sender.sendMessage(nmt_message);

  controller.autoAlignMotorPosition();
  for (int i = 0; i < 10 ; i++) {
    controller.updateActualVelocity();
    int vel = controller.getVelocity();
    log.INFO("MOT-TEST", "Motor velocity: %d", vel);
    Thread::sleep(100);
  }

  controller.enterPreOperational();
  // controller.enterPreOperational();

  // correct data needs to be configured in the controller config files:
  // controller.configure();
  // controller.enterOperational();
  // controller.sendTargetVelocity(200);

  // for (int i = 0; i < 15; i++) {
  //   controller.updateActualVelocity();
  //   uint32_t vel = controller.getVelocity();
  //   log.INFO("TEST-CONTROLLER", "Actual motor velocity: %d rpm",vel);
  //   Thread::sleep(1000);
  // }

  // 
}
