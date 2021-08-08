#include "navigation/main.hpp"

#include <embrakes/main.hpp>
#include <fstream>
#include <propulsion/main.hpp>
#include <sensors/main.hpp>
#include <state_machine/main.hpp>
#include <string>
#include <telemetry/main.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/logger.hpp>
#include <utils/system.hpp>

using hyped::utils::Logger;
using hyped::utils::System;
using hyped::utils::concurrent::Thread;

using hyped::data::Sensors;

int main(int argc, char *argv[])
{
  System::parseArgs(argc, argv);
  System &sys = System::getSystem();
  Logger log_system(sys.verbose, sys.debug);
  Logger log_motor(sys.verbose_motor, sys.debug_motor);
  Logger log_embrakes(sys.verbose_embrakes, sys.debug_embrakes);
  Logger log_nav(sys.verbose_nav, sys.debug_nav);
  Logger log_sensor(sys.verbose_sensor, sys.debug_sensor);
  Logger log_state(sys.verbose_state, sys.debug_state);
  Logger log_tlm(sys.verbose_tlm, sys.debug_tlm);

  // print HYPED logo at system startup
  std::ifstream file("main_logo.txt");
  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      printf("%s\n", line.c_str());
    }
    file.close();
  }

  log_system.INFO("MAIN", "Starting BBB with %d modules", 5);
  log_system.DBG("MAIN", "DBG0");
  log_system.DBG1("MAIN", "DBG1");
  log_system.DBG2("MAIN", "DBG2");
  log_system.DBG3("MAIN", "DBG3");

  // Initalise the threads here
  Thread *sensors       = new hyped::sensors::Main(0, log_sensor);
  Thread *embrakes      = new hyped::embrakes::Main(1, log_embrakes);
  Thread *motors        = new hyped::motor_control::Main(2, log_motor);
  Thread *state_machine = new hyped::state_machine::Main(4, log_state);
  Thread *nav           = new hyped::navigation::Main(5, log_nav);
  Thread *tlm           = new hyped::telemetry::Main(3, log_tlm);

  // Start the threads here
  sensors->start();
  embrakes->start();
  motors->start();
  state_machine->start();
  nav->start();
  tlm->start();

  // Join the threads here
  sensors->join();
  embrakes->join();
  motors->join();
  state_machine->join();
  nav->join();
  tlm->join();

  delete sensors;
  delete embrakes;
  delete motors;
  delete state_machine;
  delete nav;
  delete tlm;

  return 0;
}
