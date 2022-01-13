#include "config.hpp"
#include "system.hpp"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <csignal>
#include <cstring>

#define DEFAULT_CONFIG "config.txt"

#define DEFAULT_VERBOSE -1
#define DEFAULT_DEBUG -1
#define DEFAULT_IMU -1
#define DEFAULT_RUN -1
#define DEFAULT_QUERIES -1
#define DEFAULT_Q_DELAY -1
#define DEFAULT_NAV_ID -1

namespace hyped {
namespace utils {

namespace {
void printUsage()
{
  printf("./hyped [args]\n");
  printf(
    "All arguments are optional. To provide an argument with a value, use <argument>=<value>.\n"
    "Default value of verbose flags is 1\n"
    "Default value of debug   flags is 0\n"
    "\n  -v, --verbose[=<bool>]\n"
    "    Set system-wide setting of verbosity. If enabled, prints all INFO messages\n"
    "\n  --verbose_motor, --verbose_nav, --verbose_sensor, --verbose_state, --verbose_tlm, "
    "--verbose_brakes\n"
    "    Set module-specific setting of verbosity. If enabled, prints all INFO messages\n"
    "\n  -d, --debug[=<level>]\n"
    "    Set system-wide debug level. All DBG[n] where n <= level messages are printed.\n"
    "\n  --debug_motor, --debug_nav, --debug_sensor, --debug_state, --debug_tlm, --debug_brakes\n"
    "    Set module-specific debug level. All DBG[n] where n <= level messages are printed.\n"
    "    To use fake system.\n"
    "    --fake_imu --fake_imu_fail\n"
    "    --fake_batteries --fake_batteries_fail\n"
    "    --fake_keyence --fake_keyence_fail\n"
    "    --fake_temperature --fake_temperature_fail\n"
    "    --fake_brakes --fake_motors\n"
    "    --battery_test  --fake_highpower\n"
    "    To set navigation IDs.\n"
    "    --imu_id, --run_id\n"
    "    To set which IMU axis to be navigated after.\n"
    "    --axis\n"
    "    To set run kind for navigation tests.\n"
    "    --official_run, --elevator_run, --stationary_run, --outside_run\n"
    "    To disable telemetry module.\n"
    "    --telemetry_off\n"
    "");
}
}  // namespace

System::~System()
{
  if (log_) delete log_;
}

System::System(int argc, char *argv[])
    : verbose(false),
      verbose_motor(DEFAULT_VERBOSE),
      verbose_nav(DEFAULT_VERBOSE),
      verbose_sensor(DEFAULT_VERBOSE),
      verbose_state(DEFAULT_VERBOSE),
      verbose_tlm(DEFAULT_VERBOSE),
      verbose_brakes(DEFAULT_VERBOSE),
      debug(DEFAULT_DEBUG),
      debug_motor(DEFAULT_DEBUG),
      debug_nav(DEFAULT_DEBUG),
      debug_sensor(DEFAULT_DEBUG),
      debug_state(DEFAULT_DEBUG),
      debug_tlm(DEFAULT_DEBUG),
      debug_brakes(DEFAULT_VERBOSE),
      fake_imu(false),
      fake_batteries(false),
      fake_keyence(false),
      fake_temperature(false),
      fake_brakes(false),
      fake_motors(false),
      fake_imu_fail(false),
      fake_batteries_fail(false),
      fake_keyence_fail(false),
      fake_temperature_fail(false),
      battery_test(false),
      fake_highpower(false),
      imu_id(DEFAULT_NAV_ID),
      run_id(DEFAULT_NAV_ID),
      axis(0),
      official_run(true),
      elevator_run(false),
      stationary_run(false),
      outside_run(false),
      telemetry_off(false),
      running_(true),
      config(0)
{
  strncpy(config_file, DEFAULT_CONFIG, 250);

  int c;
  int option_index = 0;
  while (1) {
    static option long_options[]
      = {{"verbose", optional_argument, 0, 'v'},
         {"verbose_motor", optional_argument, 0, 'a'},
         {"verbose_nav", optional_argument, 0, 'A'},
         {"verbose_sensor", optional_argument, 0, 'b'},
         {"verbose_state", optional_argument, 0, 'B'},
         {"verbose_brakes", optional_argument, 0, 'o'},
         {"verbose_tlm", optional_argument, 0, 'c'},
         {"config", required_argument, 0, 'C'},
         {"debug", optional_argument, 0, 'd'},
         {"debug_motor", optional_argument, 0, 'e'},
         {"debug_nav", optional_argument, 0, 'E'},
         {"debug_sensor", optional_argument, 0, 'f'},
         {"debug_state", optional_argument, 0, 'F'},
         {"debug_tlm", optional_argument, 0, 'g'},
         {"debug_brakes", optional_argument, 0, 'O'},
         {"help", no_argument, 0, 'h'},
         {"fake_imu", no_argument, 0, 'i'},
         {"fake_batteries", no_argument, 0, 'j'},
         {"fake_keyence", no_argument, 0, 'k'},
         {"fake_temperature", no_argument, 0, 'l'},
         {"fake_motors", no_argument, 0, 'm'},
         {"fake_brakes", no_argument, 0, 'n'},
         {"fake_imu_fail", no_argument, 0, 'I'},
         {"fake_batteries_fail", no_argument, 0, 'J'},
         {"fake_keyence_fail", no_argument, 0, 'K'},
         {"fake_temperature_fail", no_argument, 0, 'L'},
         {"fake_highpower", no_argument, 0, 'z'},
         {"battery_test", no_argument, 0, 'Z'},
         {"imu_id", no_argument, 0, 'p'},
         {"run_id", no_argument, 0, 'q'},
         {"axis", required_argument, 0, 'u'},
         {"official_run", no_argument, 0, 'r'},
         {"elevator_run", no_argument, 0, 's'},
         {"stationary_run", no_argument, 0, 't'},
         {"outside_run", no_argument, 0, 'w'},
         {"telemetry_off", no_argument, 0, 'x'},
         {0, 0, 0, 0}};  // options for long in long_options array, can support optional argument
    // returns option character from argv array following '-' or '--' from command line
    c = getopt_long(argc, argv, "vd::h", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1) break;

    switch (c) {
      case 'v':
        if (optarg)
          verbose = atoi(optarg);
        else
          verbose = true;
        break;
      case 'h':
        printUsage();
        exit(0);
        break;
      case 'd':
        if (optarg)
          debug = atoi(optarg);
        else
          debug = 0;
        break;
      case 'a':  // verbose_motor
        if (optarg)
          verbose_motor = atoi(optarg);
        else
          verbose_motor = true;
        break;
      case 'A':  // verbose_nav
        if (optarg)
          verbose_nav = atoi(optarg);
        else
          verbose_nav = true;
        break;
      case 'b':  // verbose_sensor
        if (optarg)
          verbose_sensor = atoi(optarg);
        else
          verbose_sensor = true;
        break;
      case 'B':  // verbose_state
        if (optarg)
          verbose_state = atoi(optarg);
        else
          verbose_state = true;
        break;
      case 'c':  // verbose_tlm
        if (optarg)
          verbose_tlm = atoi(optarg);
        else
          verbose_tlm = true;
        break;
      case 'o':  // verbose_brakes
        if (optarg)
          verbose_brakes = atoi(optarg);
        else
          verbose_brakes = true;
        break;
      case 'C':
        strncpy(config_file, optarg, 250 - 1);
        break;
      case 'e':  // debug_motor
        if (optarg)
          debug_motor = atoi(optarg);
        else
          debug_motor = 0;
        break;
      case 'E':  // debug_nav
        if (optarg)
          debug_nav = atoi(optarg);
        else
          debug_nav = 0;
        break;
      case 'f':  // debug_sensor
        if (optarg)
          debug_sensor = atoi(optarg);
        else
          debug_sensor = 0;
        break;
      case 'F':  // debug_state
        if (optarg)
          debug_state = atoi(optarg);
        else
          debug_state = 0;
        break;
      case 'g':  // debug_tlm
        if (optarg)
          debug_tlm = atoi(optarg);
        else
          debug_tlm = 0;
        break;
      case 'O':  // debug_brakes
        if (optarg)
          debug_brakes = atoi(optarg);
        else
          debug_brakes = 0;
        break;
      case 'i':  // fake_imu
        if (optarg)
          fake_imu = atoi(optarg);
        else
          fake_imu = 1;
        break;
      case 'j':  // fake batteries
        if (optarg)
          fake_batteries = atoi(optarg);
        else
          fake_batteries = 1;
        break;
      case 'k':  // fake_keyence
        if (optarg)
          fake_keyence = atoi(optarg);
        else
          fake_keyence = 1;
        break;
      case 'l':  // fake_temeperature
        if (optarg)
          fake_temperature = atoi(optarg);
        else
          fake_temperature = 1;
        break;
      case 'm':  // fake_motors
        if (optarg)
          fake_motors = atoi(optarg);
        else
          fake_motors = 1;
        break;
      case 'n':  // fake_brakes
        if (optarg)
          fake_brakes = atoi(optarg);
        else
          fake_brakes = 1;
        break;
      case 'I':  // fake_imu_fail
        if (optarg)
          fake_imu_fail = atoi(optarg);
        else
          fake_imu_fail = 1;
        break;
      case 'J':  // fake batteries_fail
        if (optarg)
          fake_batteries_fail = atoi(optarg);
        else
          fake_batteries_fail = 1;
        break;
      case 'K':  // fake_keyence_fail
        if (optarg)
          fake_keyence_fail = atoi(optarg);
        else
          fake_keyence_fail = 1;
        break;
      case 'L':  // fake_temperature_fail
        if (optarg)
          fake_temperature_fail = atoi(optarg);
        else
          fake_temperature_fail = 1;
        break;
      case 'z':  // fake_highpower
        if (optarg)
          fake_highpower = atoi(optarg);
        else
          fake_highpower = 1;
        break;
      case 'Z':  // fake_battery_test
        if (optarg)
          battery_test = atoi(optarg);
        else
          battery_test = 1;
        break;
      case 'p':  // imu_id
        if (optarg)
          imu_id = atoi(optarg);
        else
          imu_id = 1;
        break;
      case 'q':  // run_id
        if (optarg)
          run_id = atoi(optarg);
        else
          run_id = 1;
        break;
      case 'u':  // axis
        if (optarg)
          axis = atoi(optarg);
        else
          axis = 0;
        break;
      case 'r':  // official_run
        if (optarg)
          official_run = atoi(optarg);
        else
          official_run = 1;
        break;
      case 's':  // elevator_run
        if (optarg) {
          elevator_run = atoi(optarg);
        } else {
          elevator_run = 1;
        }
        official_run = 0;
        break;
      case 't':  // stationary_run
        if (optarg) {
          stationary_run = atoi(optarg);
        } else {
          stationary_run = 1;
        }
        official_run     = 0;
        enable_nav_write = stationary_run;
        break;
      case 'w':  // outside_run
        if (optarg) {
          outside_run = atoi(optarg);
        } else {
          outside_run = 1;
        }
        official_run = 0;
        break;
      case 'x':  // telemetry_off
        if (optarg)
          telemetry_off = atoi(optarg);
        else
          telemetry_off = 1;
        break;
      default:
        printUsage();
        exit(1);
        break;
    }
  }

  // propagate verbose and debug to modules if not set module-specific
  if (verbose_motor == DEFAULT_VERBOSE) verbose_motor = verbose;
  if (verbose_nav == DEFAULT_VERBOSE) verbose_nav = verbose;
  if (verbose_sensor == DEFAULT_VERBOSE) verbose_sensor = verbose;
  if (verbose_state == DEFAULT_VERBOSE) verbose_state = verbose;
  if (verbose_tlm == DEFAULT_VERBOSE) verbose_tlm = verbose;
  if (verbose_brakes == DEFAULT_VERBOSE) verbose_brakes = verbose;

  if (debug_motor == DEFAULT_DEBUG) debug_motor = debug;
  if (debug_nav == DEFAULT_DEBUG) debug_nav = debug;
  if (debug_sensor == DEFAULT_DEBUG) debug_sensor = debug;
  if (debug_state == DEFAULT_DEBUG) debug_state = debug;
  if (debug_tlm == DEFAULT_DEBUG) debug_tlm = debug;
  if (debug_brakes == DEFAULT_DEBUG) debug_brakes = debug;

  log_    = new Logger(verbose, debug);
  system_ = this;  // own address
}

System *System::system_ = 0;

void System::parseArgs(int argc, char *argv[])
{
  if (system_) return;  // when all command-line option have been parsed

  system_ = new System(argc, argv);  // System overloaded
  if (system_->config == 0) system_->config = new Config(system_->config_file);
}

System &System::getSystem()
{
  if (system_) return *system_;
  Logger log;
  log.ERR("SYSTEM",
          "somebody tried to get System"
          " before initialisation, aborting");
  exit(1);
}

Logger &System::getLogger()
{
  System &sys = getSystem();
  return *sys.log_;
}

static void gracefulExit(int)
{
  System &sys  = System::getSystem();
  sys.running_ = false;

  Logger log(true, 0);
  log.INFO("SYSTEM", "termination signal received, exiting gracefully");
  exit(0);
}

static void segfaultHandler(int)
{
  // start turning the system off
  System &sys  = System::getSystem();
  sys.running_ = false;

  Logger log(true, 0);
  log.ERR("SYSTEM", "forced termination detected (segfault?)");
  exit(0);
}

bool System::setExitFunction()
{
  static bool signal_set = false;
  if (signal_set) return true;

  // nominal termination
  std::signal(SIGINT, &gracefulExit);

  // forced termination
  std::signal(SIGSEGV, &segfaultHandler);
  std::signal(SIGABRT, &segfaultHandler);
  std::signal(SIGFPE, &segfaultHandler);
  std::signal(SIGILL, &segfaultHandler);
  std::signal(SIGTERM, &segfaultHandler);

  signal_set = true;
  return true;
}

bool handle_registeres = System::setExitFunction();
}  // namespace utils
}  // namespace hyped
