#include <thread>
#include "data/data.hpp"
#include "telemetry/main.hpp"
#include "sensors/main.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"

using hyped::utils::Logger;
using hyped::utils::System;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;

void loop(Logger& logger);

int main(int argc, char* argv[]) {
    System::parseArgs(argc, argv);
    System& sys = System::getSystem();
    Logger log_tlm(sys.verbose_tlm, sys.debug_tlm);
    Logger log_sensor(sys.verbose_sensor, sys.debug_sensor);

    std::thread loopThread {loop, std::ref(log_tlm)};

    Thread* telemetry = new hyped::telemetry::Main(4, log_tlm);
    Thread* sensors = new hyped::sensors::Main(0, log_sensor);

    telemetry->start();
    sensors->start();
    sensors->join();
    telemetry->join();

    delete sensors;
    delete telemetry;

    loopThread.join();
}

void loop(Logger& logger) {
    Data& data = Data::getInstance();

    while (true) {
        Navigation nav_data                     = data.getNavigationData();
        StateMachine sm_data                    = data.getStateMachineData();
        Motors motor_data                       = data.getMotorData();
        EmergencyBrakes emergency_brakes_data   = data.getEmergencyBrakesData();
        Telemetry telem_data                    = data.getTelemetryData();

        logger.DBG2("Telemetry", "SHARED module_status: %d", telem_data.module_status);
        logger.DBG2("Telemetry", "SHARED launch_command: %s", telem_data.launch_command ? "true" : "false"); // NOLINT
        logger.DBG2("Telemetry", "SHARED calibrate_command: %s", telem_data.calibrate_command ? "true" : "false"); // NOLINT
        logger.DBG2("Telemetry", "SHARED reset_command: %s", telem_data.reset_command ? "true" : "false"); // NOLINT
        logger.DBG2("Telemetry", "SHARED service_propulsion_go: %s", telem_data.service_propulsion_go ? "true" : "false"); // NOLINT
        logger.DBG2("Telemetry", "SHARED emergency_stop_command: %s", telem_data.emergency_stop_command ? "true" : "false"); // NOLINT

        nav_data.module_status = ModuleStatus::kReady;
        nav_data.distance = 111;
        nav_data.velocity = 111;
        nav_data.acceleration = 111;
        data.setNavigationData(nav_data);

        sm_data.current_state = kReady;
        data.setStateMachineData(sm_data);

        motor_data.velocity_1 = 101;
        motor_data.velocity_2 = 102;
        data.setMotorData(motor_data);

        emergency_brakes_data.front_brakes = false;
        emergency_brakes_data.rear_brakes = false;
        data.setEmergencyBrakesData(emergency_brakes_data);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        nav_data.module_status = ModuleStatus::kInit;
        nav_data.distance = 222;
        nav_data.velocity = 222;
        nav_data.acceleration = 222;
        data.setNavigationData(nav_data);

        sm_data.current_state = kCalibrating;
        data.setStateMachineData(sm_data);

        motor_data.velocity_1 = 201;
        motor_data.velocity_2 = 202;
        data.setMotorData(motor_data);

        emergency_brakes_data.front_brakes = true;
        emergency_brakes_data.rear_brakes = true;
        data.setEmergencyBrakesData(emergency_brakes_data);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
