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

void ImuDataLogger::dataToFileSimulation(data::NavigationVector &acceleration, uint32_t timestamp)
{
  *outfile_ << acceleration[0] << ",";
  *outfile_ << acceleration[1] << ",";
  *outfile_ << acceleration[2] << ",";
  *outfile_ << timestamp << std::endl;
}

void ImuDataLogger::dataToFile(data::NavigationVector &raw_acceleration,
                               data::NavigationVector &calibrated_acceleration, uint32_t timestamp)
{
  *outfile_ << raw_acceleration[0] << ",";
  *outfile_ << raw_acceleration[1] << ",";
  *outfile_ << raw_acceleration[2] << ",";
  *outfile_ << calibrated_acceleration[0] << ",";
  *outfile_ << calibrated_acceleration[1] << ",";
  *outfile_ << calibrated_acceleration[2] << ",";
  *outfile_ << timestamp << std::endl;
}

void ImuDataLogger::dataToFileKalman(data::NavigationVector &raw_acceleration,
                                     data::NavigationVector &calibrated_acceleration,
                                     data::NavigationVector &x, uint32_t timestamp)
{
  *outfile_ << raw_acceleration[0] << ",";
  *outfile_ << raw_acceleration[1] << ",";
  *outfile_ << raw_acceleration[2] << ",";
  *outfile_ << calibrated_acceleration[0] << ",";
  *outfile_ << calibrated_acceleration[1] << ",";
  *outfile_ << calibrated_acceleration[2] << ",";
  *outfile_ << x[0] << ",";
  *outfile_ << x[1] << ",";
  *outfile_ << x[2] << ",";
  *outfile_ << timestamp << std::endl;
}

}  // namespace navigation
}  // namespace hyped
