#include "imu_data_logger.hpp"

namespace hyped {
namespace navigation {

ImuDataLogger::ImuDataLogger() : file_path_(), outfile_(new std::ofstream())
{
}

ImuDataLogger::~ImuDataLogger()
{
  outfile_->close();
}

void ImuDataLogger::setup(int imu_id, int run_id)
{
  char buff[100];
  snprintf(buff, sizeof(buff), "test_data/run%d/imu%d/data.csv", run_id, imu_id);
  file_path_ = buff;
  outfile_->open(file_path_);
  *outfile_ << "arx,ary,arz,acx,acy,acz,t\n";
}

void ImuDataLogger::setupKalman(int imu_id, int run_id)
{
  char buff[100];
  snprintf(buff, sizeof(buff), "test_data/run%d/imu%d/data.csv", run_id, imu_id);
  file_path_ = buff;
  outfile_->open(file_path_);
  *outfile_ << "arx,ary,arz,acx,acy,acz,afx,afy,afz,t\n";
}

void ImuDataLogger::dataToFileSimulation(NavigationVector &acc, uint32_t timestamp)
{
  *outfile_ << acc[0] << "," << acc[1] << "," << acc[2] << "," << timestamp << "\n";
}

void ImuDataLogger::dataToFile(NavigationVector &accR, NavigationVector &accC, uint32_t timestamp)
{
  *outfile_ << accR[0] << "," << accR[1] << "," << accR[2] << "," << accC[0] << "," << accC[1]
            << "," << accC[2] << "," << timestamp << "\n";
}

void ImuDataLogger::dataToFileKalman(NavigationVector &accR, NavigationVector &accC,
                                     NavigationVector &x, uint32_t timestamp)
{
  *outfile_ << accR[0] << "," << accR[1] << "," << accR[2] << "," << accC[0] << "," << accC[1]
            << "," << accC[2] << "," << x[0] << "," << x[1] << "," << x[2] << "," << timestamp
            << "\n";
}

}  // namespace navigation
}  // namespace hyped
