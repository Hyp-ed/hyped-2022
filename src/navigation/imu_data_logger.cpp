#include "imu_data_logger.hpp"

#include <fstream>
#include <sstream>

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
  std::ostringstream stream;
  stream << "test_data/run" << run_id << "/imu" << imu_id << "/data.csv";
  file_path_ = stream.str();
  outfile_->open(file_path_);
  *outfile_ << "arx,ary,arz,acx,acy,acz,t" << std::endl;
}

void ImuDataLogger::setupKalman(const int imu_id, const int run_id)
{
  std::ostringstream stream;
  stream << "test_data/run" << run_id << "/imu" << imu_id << "/data.csv";
  file_path_ = stream.str();
  outfile_->open(file_path_);
  *outfile_ << "arx,ary,arz,acx,acy,acz,afx,afy,afz,t" << std::endl;
}

void ImuDataLogger::dataToFileSimulation(const data::NavigationVector &acceleration,
                                         const uint32_t timestamp)
{
  for (std::size_t i = 0; i < 3; i++) {
    *outfile_ << acceleration[i] << ",";
  }
  *outfile_ << timestamp << std::endl;
}

void ImuDataLogger::dataToFile(const data::NavigationVector &raw_acceleration,
                               const data::NavigationVector &calibrated_acceleration,
                               const uint32_t timestamp)
{
  for (std::size_t i = 0; i < 3; i++) {
    *outfile_ << raw_acceleration[i] << ",";
  }
  for (std::size_t i = 0; i < 3; i++) {
    *outfile_ << calibrated_acceleration[i] << ",";
  }
  *outfile_ << timestamp << std::endl;
}

void ImuDataLogger::dataToFileKalman(const data::NavigationVector &raw_acceleration,
                                     const data::NavigationVector &calibrated_acceleration,
                                     const data::NavigationVector &x, const uint32_t timestamp)
{
  for (std::size_t i = 0; i < 3; i++) {
    *outfile_ << raw_acceleration[i] << ",";
  }
  for (std::size_t i = 0; i < 3; i++) {
    *outfile_ << calibrated_acceleration[i] << ",";
  }
  for (std::size_t i = 0; i < 3; i++) {
    *outfile_ << x[i] << ",";
  }
  *outfile_ << timestamp << std::endl;
}

}  // namespace navigation
}  // namespace hyped
