// demo file to spin up the motors by running the main propulsion thread.
#include <stdio.h>

#include <vector>

#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
#include "propulsion/main.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::data::StateMachine;
using hyped::data::Motors;
using hyped::data::ModuleStatus;
using hyped::data::Data;
using hyped::data::State;
using hyped::data::Navigation;
using hyped::data::NavigationType;
using hyped::motor_control::Main;

#define VEL_OFFSET 1024.30824

std::vector<NavigationType> readVel(const char* filepath);

int main(int argc, char** argv)
{
  System::parseArgs(argc, argv);
  Logger& log = System::getLogger();
  Data& data = Data::getInstance();
  StateMachine state = data.getStateMachineData();
  Motors motor_data = data.getMotorData();
  Navigation nav_data = data.getNavigationData();

  state.current_state = State::kIdle;
  data.setStateMachineData(state);

  Main* motors = new Main(1, log);
  motors->start();

  std::vector<NavigationType> vels = readVel("data/in/velocities-2motors-noPowerLoss-100ms.txt");

  while (motor_data.module_status != ModuleStatus::kInit) {
    motor_data = data.getMotorData();
  }
  if (motor_data.module_status == ModuleStatus::kInit) {
    state.current_state = State::kCalibrating;
    data.setStateMachineData(state);
  }

  while (motor_data.module_status != ModuleStatus::kReady) {
    motor_data = data.getMotorData();
  }
  if (motor_data.module_status == ModuleStatus::kReady) {
    char ch;
    puts("Preparing to accelerate. Press ENTER to continue...");
    scanf("%c",&ch);

    state.current_state = State::kAccelerating;
    data.setStateMachineData(state);
  }

  uint i = 0;

  while (state.current_state == State::kAccelerating) {
    nav_data = data.getNavigationData();

    nav_data.velocity = vels.at(i);
    data.setNavigationData(nav_data);

    Thread::sleep(100);
    i++;

    if (i >= vels.size()) {
      break;
    }
    state = data.getStateMachineData();
  }

  log.INFO("TEST", "Shutting down");
  motors->join();
}

std::vector<NavigationType> readVel(const char* filepath)
{
  std::vector<NavigationType> velocities;
  FILE* fp;
  fp = fopen(filepath, "r");

  if (fp == NULL) {
    return velocities;
  } else {
    char line[250];
    while (fgets(line, static_cast<int>(sizeof(line)/sizeof(line[0])), fp) != NULL) {
      velocities.push_back(std::atof(line));
    }
  }
  return velocities;
  fclose(fp);
}