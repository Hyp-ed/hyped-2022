#pragma once
#include <cmath>

#include <vector>

namespace hyped::utils::math {

class Regression {
 public:
  Regression();

  /**
   * @brief calculates regressed coefficients using basic x-y linear regression.
   * only works for straight lines!
   *
   * @param x_data vector of x-data points
   * @param y_data vector of y-data points
   */
  void CaulculateCoefficients(std::vector<double> x_data, std::vector<double> y_data);

  // calculated coefficients from regression function
  std::vector<double> coefficients_;
};
}  // namespace hyped::utils::math