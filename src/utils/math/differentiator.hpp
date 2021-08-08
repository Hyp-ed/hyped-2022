#pragma once

#include <data/data_point.hpp>

namespace hyped {
namespace utils {
namespace math {

using hyped::data::DataPoint;

template<typename T>
class Differentiator {
 public:
  Differentiator();

  /**
   * @brief    Calculates the gradient given two points for time T_(N)
   *
   * @param[in]  point1    The point for time T_(N-2)
   * @param[in]  point2    The point for time T_(N-1)
   */
  DataPoint<T> update(DataPoint<T> point);

 private:
  DataPoint<T> prev_point_;
  bool initialised_;
};

template<typename T>
Differentiator<T>::Differentiator() : prev_point_(0, T(0)),
                                      initialised_(false)
{
}

template<typename T>
DataPoint<T> Differentiator<T>::update(DataPoint<T> point)
{
  if (!initialised_) {
    prev_point_  = point;
    initialised_ = true;
    return DataPoint<T>(point.timestamp, T(0));
  }
  // Assume timestamp in microseconds and convert to seconds
  T gradient
    = (point.value - prev_point_.value) / ((point.timestamp - prev_point_.timestamp) / 1e6);

  prev_point_ = point;

  return DataPoint<T>(point.timestamp, gradient);
}

}  // namespace math
}  // namespace utils
}  // namespace hyped
