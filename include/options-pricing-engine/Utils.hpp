#pragma once
#include <cmath>

namespace utils {
inline double normalCDF(double x) {
  return 0.5 * std::erfc(-x / std::sqrt(2.0));
}
inline double normalPDF(double x) {
  return (1.0 / std::sqrt(2.0 * M_PI)) * std::exp(-0.5 * x * x);
}
} // namespace utils