#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "propulsion/controller.hpp"
#include "sensors/bms_manager.hpp"
#include "data/data.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::motor_control::Controller;
using hyped::sensors::BmsManager;
using hyped::data::Data;
using hyped::data::StateMachine;
using hyped::data::State;
using hyped::data::Batteries;

int main(int argc, char** argv) {
  System::parseArgs(argc, argv);
  Logger log = System::getLogger();

  Data& data_ = Data::getInstance();
  BmsManager bms_manager(log);
  bms_manager.start();
  Thread::sleep(500);
  StateMachine state_machine = data_.getStateMachineData();

  // Create and initialise motors
  Controller* controller = new Controller(log, 1);
  // Register the controllers as nodes on the CANBus
  controller->registerController();

  // Send Configuration messages to controllers
  controller->configure();

  log.INFO("TEST", "Transitioning to kReady in 3 seconds");
  Thread::sleep(3000);

  state_machine.current_state = State::kReady;
  data_.setStateMachineData(state_machine);

  // Send auto align command
  controller->autoAlignMotorPosition();

  // Sleep to process any errors
  Thread::sleep(100000);
}