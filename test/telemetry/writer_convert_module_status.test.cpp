#include <string>

#include <data/data.hpp>
#include <gtest/gtest.h>
#include <telemetry/writer.hpp>

namespace hyped::testing {

/**
 * Tests the conversion of Module Status from Enum to String as required for GUI
 */
class WriterConvertModuleStatus : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(WriterConvertModuleStatus, convertsStartStatus)
{
  std::string convertedModuleStatus
    = telemetry::Writer::convertModuleStatus(data::ModuleStatus::kStart);
  ASSERT_EQ(convertedModuleStatus, "START") << "Should convert Start status.";
}

TEST_F(WriterConvertModuleStatus, convertsInitStatus)
{
  std::string convertedModuleStatus
    = telemetry::Writer::convertModuleStatus(data::ModuleStatus::kInit);
  ASSERT_EQ(convertedModuleStatus, "INIT") << "Should convert Init status.";
}

TEST_F(WriterConvertModuleStatus, convertsReadyStatus)
{
  std::string convertedModuleStatus
    = telemetry::Writer::convertModuleStatus(data::ModuleStatus::kReady);
  ASSERT_EQ(convertedModuleStatus, "READY") << "Should convert Ready status.";
}

TEST_F(WriterConvertModuleStatus, convertsCriticalFailureState)
{
  std::string convertedModuleStatus
    = telemetry::Writer::convertModuleStatus(data::ModuleStatus::kCriticalFailure);
  ASSERT_EQ(convertedModuleStatus, "CRITICAL_FAILURE") << "Should convert Critical Failure status.";
}

}  // namespace hyped::testing
