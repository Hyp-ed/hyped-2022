#pragma once

// TODO(Uday): Decide the process noise value

#include <utils/math/vector.hpp>
namespace hyped {
namespace utils {
namespace math {

/**
 * @brief    This class is for filtering the data from sensors to smoothen it.
 *
 * @param[in] T    This is the type of the data value that it should filter.
 */
template<typename T>
class Kalman {
 public:
  /**
   * @brief    Construct a new Kalman object without any configurations
   */
  Kalman();

  /**
   * @brief    Construct a new Kalman object
   *
   * @param[in] input_value    Initial value of the reading
   * @param[in] measurement_noise    Standard deviation of the measurement
   * @param[in] process_noise    How noisy the measurement can be (predetermined)
   */
  Kalman(T input_value, T measurement_noise, T process_noise);

  /**
   * @brief    Configures the variables in the Kalman object
   *
   * @param[in] input_value    Initial value of the reading
   * @param[in] measurement_noise    Standard deviation of the measurement
   * @param[in] process_noise    How noisy the measurement can be (predetermined)
   */
  void configure(T input_value, T measurement_noise, T process_noise);

  /**
   * @brief    Filters a value based on it's previous values
   *
   * @param[in] input    Value that needs to be filtered
   *
   * @return    Returns the filtered value
   */
  T filter(const T &input);

  /**
   * @brief    Get the Filtered object
   *
   * @return    Returns the last filtered value
   */
  T getFiltered();

 private:
  T kalman_gain_;
  T process_noise_;
  T filtered_value_;
  T estimation_error_covariance_;
  T measurement_noise_covariance_;
};

template<typename T>
Kalman<T>::Kalman()
    : kalman_gain_(),
      process_noise_(),
      filtered_value_(),
      estimation_error_covariance_(),
      measurement_noise_covariance_()
{
}

template<typename T>
Kalman<T>::Kalman(T input_value, T measurement_noise, T process_noise)
    : kalman_gain_(),
      process_noise_(process_noise),
      filtered_value_(input_value),
      estimation_error_covariance_(),
      measurement_noise_covariance_(measurement_noise)
{
}

template<typename T>
void Kalman<T>::configure(T input_value, T measurement_noise, T process_noise)
{
  filtered_value_               = input_value;
  process_noise_                = process_noise;
  measurement_noise_covariance_ = measurement_noise;
}

template<typename T>
T Kalman<T>::filter(const T &input)
{
  estimation_error_covariance_ += process_noise_;
  kalman_gain_
    = estimation_error_covariance_ / (estimation_error_covariance_ + measurement_noise_covariance_);
  filtered_value_ += kalman_gain_ * (input - filtered_value_);
  estimation_error_covariance_ = (1 - kalman_gain_) * estimation_error_covariance_;

  return filtered_value_;
}

template<typename T>
T Kalman<T>::getFiltered()
{
  return filtered_value_;
}

}  // namespace math
}  // namespace utils
}  // namespace hyped
