/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 31/03/2019
 * Description: Multivariate Kalman filter implementation to filter sensor measurement
 *              considering the dynamics of the system
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.

 */

#include "kalman_multivariate.hpp"

namespace hyped {
    namespace utils {
        namespace math {

            KalmanMultivariate::KalmanMultivariate(unsigned int n, unsigned int m)
                : n_(n),
                  m_(m),
                  k_(0)
            {}

            KalmanMultivariate::KalmanMultivariate(unsigned int n, unsigned int m,
                                                   unsigned int k)
                : n_(n),
                  m_(m),
                  k_(k)
            {}

            void KalmanMultivariate::setDynamicsModel(MatrixXf& A, MatrixXf& Q)
            {
                A_ = A;
                Q_ = Q;
            }

            void KalmanMultivariate::setDynamicsModel(MatrixXf& A, MatrixXf& B, MatrixXf& Q)
            {
                A_ = A;
                B_ = B;
                Q_ = Q;
            }

            void KalmanMultivariate::setMeasurementModel(MatrixXf& H, MatrixXf& R)
            {
                H_ = H;
                R_ = R;
            }

            void KalmanMultivariate::setModels(MatrixXf& A, MatrixXf& Q, MatrixXf& H,
                                       MatrixXf& R)
            {
                setDynamicsModel(A, Q);
                setMeasurementModel(H, R);
            }

            void KalmanMultivariate::setModels(MatrixXf& A, MatrixXf& B, MatrixXf& Q,
                                       MatrixXf& H, MatrixXf& R)
            {
                setDynamicsModel(A, B, Q);
                setMeasurementModel(H, R);
            }

            void KalmanMultivariate::updateA(MatrixXf& A)
            {
                A_ = A;
            }

            void KalmanMultivariate::updateR(MatrixXf& R)
            {
                R_ = R;
            }

            void KalmanMultivariate::setInitial(VectorXf& x0, MatrixXf& P0)
            {
                x_ = x0;
                P_ = P0;
                I_ = MatrixXf::Identity(n_, n_);
            }

            void KalmanMultivariate::predict()
            {
                x_ = A_ * x_;
                P_ = A_ * P_ * A_.transpose() + Q_;
            }

            void KalmanMultivariate::predict(VectorXf& u)
            {
                x_ = A_ * x_ + B_ * u;
                P_ = (A_ * P_ * A_.transpose()) + Q_;
            }

            void KalmanMultivariate::correct(VectorXf& z)
            {
                MatrixXf K = (P_ * H_.transpose()) * (H_ * P_ * H_.transpose() + R_).inverse();
                x_ = x_ + K * (z - H_ * x_);
                P_ = (I_ - K * H_) * P_;
            }

            void KalmanMultivariate::filter(VectorXf& z)
            {
                predict();
                correct(z);
            }

            void KalmanMultivariate::filter(VectorXf& u, VectorXf& z)
            {
                predict(u);
                correct(z);
            }

            VectorXf& KalmanMultivariate::getStateEstimate()
            {
                return x_;
            }

            MatrixXf& KalmanMultivariate::getStateCovariance()
            {
                return P_;
            }

        }
    }
}
