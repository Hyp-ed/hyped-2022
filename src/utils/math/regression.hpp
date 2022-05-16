#pragma once
#include <cmath>

#include <vector>

namespace hyped::utils::math {

class Regression {
 public:
  Regression();
  void GetCoeffs(std::vector<double> x_data, std::vector<double> y_data);

  struct Coefficients {
    double beta0;
    double beta1;
  };
  Coefficients coefficients;

 private:
  std::vector<double> coefficients_;
};
}  // namespace hyped::utils::math