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

void ImuDataLogger::setup(const int imu_id, const int run_id)
{
  char buff[100];
  snprintf(buff, sizeof(buff), "test_data/run%d/imu%d/data.csv", run_id, imu_id);
  file_path_ = buff;
  outfile_->open(file_path_);
  *outfile_ << "arx,ary,arz,acx,acy,acz,t\n";
}

void ImuDataLogger::setupKalman(const int imu_id, const int run_id)
{
  char buff[100];
  snprintf(buff, sizeof(buff), "test_data/run%d/imu%d/data.csv", run_id, imu_id);
  file_path_ = buff;
  outfile_->open(file_path_);
  *outfile_ << "arx,ary,arz,acx,acy,acz,afx,afy,afz,t\n";
}

void ImuDataLogger::dataToFileSimulation(data::NavigationVector &acceleration, uint32_t timestamp)
{
  *outfile_ << acceleration[0] << "," << acceleration[1] << "," << acceleration[2] << ","
            << timestamp << "\n";
}

void ImuDataLogger::dataToFile(data::NavigationVector &raw_acceleration,
                               data::NavigationVector &calibrated_acceleration, uint32_t timestamp)
{
  *outfile_ << raw_acceleration[0] << "," << raw_acceleration[1] << "," << raw_acceleration[2]
            << "," << calibrated_acceleration[0] << "," << calibrated_acceleration[1] << ","
            << calibrated_acceleration[2] << "," << timestamp << "\n";
}

void ImuDataLogger::dataToFileKalman(data::NavigationVector &raw_acceleration,
                                     data::NavigationVector &calibrated_acceleration,
                                     data::NavigationVector &x, uint32_t timestamp)
{
  *outfile_ << raw_acceleration[0] << "," << raw_acceleration[1] << "," << raw_acceleration[2]
            << "," << calibrated_acceleration[0] << "," << calibrated_acceleration[1] << ","
            << calibrated_acceleration[2] << "," << x[0] << "," << x[1] << "," << x[2] << ","
            << timestamp << "\n";
}

}  // namespace navigation
}  // namespace hyped
