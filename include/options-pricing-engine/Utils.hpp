#pragma once
#include <cmath>

namespace utils {
inline double normalCDF(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}
inline double normalPDF(double x) {
    return (1.0 / std::sqrt(2.0 * M_PI)) * std::exp(-0.5 * x * x);
}
inline double d1(double S, double K, double r, double sigma, double T) {
    return (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) /
           (sigma * std::sqrt(T));
}
inline double d2(double d1, double sigma, double T) {
    return d1 - sigma * std::sqrt(T);
}
} // namespace utils