#include <string>

#include <data/data.hpp>
#include <gtest/gtest.h>
#include <telemetry/writer.hpp>

using namespace hyped::telemetry;
using namespace hyped::data;

/**
 * Tests the conversion of Module Status from Enum to String as required for GUI
 */
struct WriterConvertModuleStatus : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(WriterConvertModuleStatus, convertsStartStatus)
{
  std::string convertedModuleStatus = Writer::convertModuleStatus(ModuleStatus::kStart);
  ASSERT_EQ(convertedModuleStatus, "START") << "Should convert Start status.";
}

TEST_F(WriterConvertModuleStatus, convertsInitStatus)
{
  std::string convertedModuleStatus = Writer::convertModuleStatus(ModuleStatus::kInit);
  ASSERT_EQ(convertedModuleStatus, "INIT") << "Should convert Init status.";
}

TEST_F(WriterConvertModuleStatus, convertsReadyStatus)
{
  std::string convertedModuleStatus = Writer::convertModuleStatus(ModuleStatus::kReady);
  ASSERT_EQ(convertedModuleStatus, "READY") << "Should convert Ready status.";
}

TEST_F(WriterConvertModuleStatus, convertsCriticalFailureState)
{
  std::string convertedModuleStatus = Writer::convertModuleStatus(ModuleStatus::kCriticalFailure);
  ASSERT_EQ(convertedModuleStatus, "CRITICAL_FAILURE") << "Should convert Critical Failure status.";
}
