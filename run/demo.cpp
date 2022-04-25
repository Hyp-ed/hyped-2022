#include <fstream>
#include <iostream>
#include <memory>

#include <brakes/main.hpp>
#include <navigation/main.hpp>
#include <propulsion/main.hpp>
#include <sensors/main.hpp>
#include <demo_state_machine/main.hpp>
#include <demo_state_machine/UI.hpp>
#include <telemetry/main.hpp>
#include <utils/system.hpp>

int main(int argc, char *argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  // print HYPED logo at system startup
  std::ifstream file("main_logo.txt");
  if (file.is_open()) {
    std::string buffer;
    file >> buffer;
    std::cout << buffer;
  }

  // Initalise the threads here
  hyped::brakes::Main brakes;
  hyped::navigation::Main navigation;
  hyped::propulsion::Main propulsion;
  hyped::sensors::Main sensors;
  hyped::demo_state_machine::Main demo_state_machine;

  // Start the threads here
  brakes.start();
  navigation.start();
  propulsion.start();
  sensors.start();
  demo_state_machine.start();

  hyped::demo_state_machine::Ui Ui; 
  Ui.run(); 

  // Join the threads here
  brakes.join();
  navigation.join();
  propulsion.join();
  sensors.join();
  demo_state_machine.join();

  return 0;
}
