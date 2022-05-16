#include "regression.hpp"

namespace hyped::utils::math {

void Regression::GetCoeffs(const std::vector<double> x_data, const std::vector<double> y_data)
{
  double x_sum = 0;
  double y_sum = 0;

  for (size_t i = 0; i < x_data.size(); ++i) {
    x_sum += x_data.at(i);
    y_sum += y_data.at(i);
  }

  double x_mean = static_cast<double>(x_sum / x_data.size());
  double y_mean = static_cast<double>(y_sum / y_data.size());
  double s_xx   = 0;
  double s_yy   = 0;
  double s_xy   = 0;

  for (size_t i = 0; i < x_data.size(); ++i) {
    s_xx += std::pow(x_data.at(i) - x_mean, 2);
    s_yy += std::pow(y_data.at(i) - y_mean, 2);
    s_xy += (x_data.at(i) - x_mean) * (y_data.at(i) - y_mean);
  }
  // regressed function will have form y = beta1*x + beta0
  double beta1 = s_xy / s_xx;
  double beta0 = y_mean - (beta1 - x_mean);

  std::vector<double> coeffs{beta1, beta0};
  coefficients.beta0 = coeffs.at(1);
  coefficients.beta1 = coeffs.at(0);
}
}  // namespace hyped::utils::math