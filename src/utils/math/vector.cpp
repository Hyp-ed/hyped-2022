#pragma once

#include <cmath>

#include <array>
#include "vector.hpp"
#include <initializer_list>
namespace hyped {
namespace utils {
namespace math {
    
template<typename T, int dimension>
Vector<T, dimension>::Vector()
{
  for (int i = 0; i < dimension; i++)
    elements_[i] = 0;
}

template<typename T, int dimension>
Vector<T, dimension>::Vector(const T element)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] = element;
}

template<typename T, int dimension>
Vector<T, dimension>::Vector(const std::array<T, dimension> &vector)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] = vector[i];
}

template<typename T, int dimension>
Vector<T, dimension>::Vector(const std::initializer_list<T> elements)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] = *(elements.begin() + i);
}

template<typename T, int dimension>
template<typename U>
Vector<T, dimension>::Vector(const Vector<U, dimension> &rhs)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] = T(rhs[i]);
}

template<typename T, int dimension>
T &Vector<T, dimension>::operator[](int index)
{
  return elements_[index];
}

template<typename T, int dimension>
T Vector<T, dimension>::operator[](int index) const
{
  return elements_[index];
}

template<typename T, int dimension>
Vector<T, dimension> Vector<T, dimension>::operator-() const
{
  return (Vector<T, dimension>() - *this);
}

template<typename T, int dimension>
Vector<T, dimension> &Vector<T, dimension>::operator+=(const Vector<T, dimension> &rhs)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] += rhs[i];
  return *this;
}

template<typename T, int dimension>
Vector<T, dimension> &Vector<T, dimension>::operator-=(const Vector<T, dimension> &rhs)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] -= rhs[i];
  return *this;
}

template<typename T, int dimension>
Vector<T, dimension> &Vector<T, dimension>::operator+=(const T rhs)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] += rhs;
  return *this;
}

template<typename T, int dimension>
Vector<T, dimension> &Vector<T, dimension>::operator-=(const T rhs)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] -= rhs;
  return *this;
}

template<typename T, int dimension>
Vector<T, dimension> &Vector<T, dimension>::operator*=(const T rhs)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] *= rhs;
  return *this;
}

template<typename T, int dimension>
Vector<T, dimension> &Vector<T, dimension>::operator*=(const Vector<T, dimension> &rhs)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] *= rhs[i];
  return *this;
}

template<typename T, int dimension>
Vector<T, dimension> &Vector<T, dimension>::operator/=(const T rhs)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] /= rhs;
  return *this;
}

template<typename T, int dimension>
Vector<T, dimension> &Vector<T, dimension>::operator/=(const Vector<T, dimension> &rhs)
{
  for (int i = 0; i < dimension; i++)
    elements_[i] /= rhs[i];
  return *this;
}

template<typename T, int dimension>
double Vector<T, dimension>::norm()
{
  double ans = 0;
  for (int i = 0; i < dimension; i++)
    ans += elements_[i] * elements_[i];
  return std::sqrt(ans);
}

template<typename T, int dimension>
Vector<T, dimension> Vector<T, dimension>::sqrt()
{
  auto ans = *this;
  for (int i = 0; i < dimension; i++)
    ans[i] = std::sqrt(ans[i]);
  return ans;
}

template<typename T, int dimension>
Vector<double, dimension> Vector<T, dimension>::toUnitVector()
{
  Vector<double, dimension> ans(*this);
  ans /= this->norm();
  return ans;
}

template<typename T1, typename T2, int dimension>
auto operator+(const Vector<T1, dimension> &lhs, const Vector<T2, dimension> &rhs)
  -> Vector<decltype(lhs[0] + rhs[0]), dimension>
{
  Vector<decltype(lhs[0] + rhs[0]), dimension> ans(lhs);
  ans += rhs;
  return ans;
}

template<typename T1, typename T2, int dimension>
auto operator+(const Vector<T1, dimension> &lhs, const T2 rhs)
  -> Vector<decltype(lhs[0] + rhs), dimension>
{
  Vector<decltype(lhs[0] + rhs), dimension> ans(lhs);
  ans += rhs;
  return ans;
}

template<typename T1, typename T2, int dimension>
auto operator+(const T1 lhs, const Vector<T2, dimension> &rhs)
  -> Vector<decltype(lhs + rhs[0]), dimension>
{
  Vector<decltype(lhs + rhs[0]), dimension> ans(lhs);
  ans += rhs;
  return ans;
}

template<typename T1, typename T2, int dimension>
auto operator-(const Vector<T1, dimension> &lhs, const Vector<T2, dimension> &rhs)
  -> Vector<decltype(lhs[0] - rhs[0]), dimension>
{
  Vector<decltype(lhs[0] - rhs[0]), dimension> ans(lhs);
  ans -= rhs;
  return ans;
}

template<typename T1, typename T2, int dimension>
auto operator-(const Vector<T1, dimension> &lhs, const T2 rhs)
  -> Vector<decltype(lhs[0] - rhs), dimension>
{
  Vector<decltype(lhs[0] - rhs), dimension> ans(lhs);
  ans -= rhs;
  return ans;
}

template<typename T1, typename T2, int dimension>
auto operator-(const T1 lhs, const Vector<T2, dimension> &rhs)
  -> Vector<decltype(lhs - rhs[0]), dimension>
{
  Vector<decltype(lhs - rhs[0]), dimension> ans(lhs);
  ans -= rhs;
  return ans;
}

template<typename T1, typename T2, int dimension>
auto operator*(const Vector<T1, dimension> &lhs, const Vector<T2, dimension> &rhs)
  -> Vector<decltype(lhs[0] * rhs[0]), dimension>
{
  Vector<decltype(lhs[0] * rhs[0]), dimension> ans(lhs);
  ans *= rhs;
  return ans;
}

template<typename T1, typename T2, int dimension>
auto operator*(const Vector<T1, dimension> &lhs, const T2 rhs)
  -> Vector<decltype(lhs[0] * rhs), dimension>
{
  Vector<decltype(lhs[0] * rhs), dimension> ans(lhs);
  ans *= rhs;
  return ans;
}

template<typename T1, typename T2, int dimension>
auto operator*(const T1 lhs, const Vector<T2, dimension> &rhs)
  -> Vector<decltype(lhs * rhs[0]), dimension>
{
  Vector<decltype(lhs * rhs[0]), dimension> ans(lhs);
  ans *= rhs;
  return ans;
}

template<typename T1, typename T2, int dimension>
auto operator/(const Vector<T1, dimension> &lhs, const T2 rhs)
  -> Vector<decltype(lhs[0] * rhs), dimension>
{
  Vector<decltype(lhs[0] / rhs), dimension> ans(lhs);
  ans /= rhs;
  return ans;
}

template<typename T1, typename T2, int dimension>
auto operator/(const Vector<T1, dimension> &lhs, const Vector<T2, dimension> &rhs)
  -> Vector<decltype(lhs[0] * rhs[0]), dimension>
{
  Vector<decltype(lhs[0] / rhs[0]), dimension> ans(lhs);
  ans /= rhs;
  return ans;
}

template<typename T1, typename T2, int dimension>
bool operator==(const Vector<T1, dimension> &lhs, const Vector<T2, dimension> &rhs)
{
  for (int i = 0; i < dimension; i++)
    if (lhs[i] != rhs[i]) return false;
  return true;
}

}  // namespace math
}  // namespace utils
}  // namespace hyped
