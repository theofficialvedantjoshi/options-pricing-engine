#pragma once
#include <cmath>
#include <random>
#include <vector>

namespace utils {

static double stepSize = 1;

inline double normalCDF(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}
inline double normalPDF(double x) {
    return (1.0 / std::sqrt(2.0 * M_PI)) * std::exp(-0.5 * x * x);
}
inline std::vector<double> generateSamples(const int N) {
    std::vector<double> samples(N);
    std::random_device rD;
    std::mt19937 generator(rD());
    std::normal_distribution<double> distribution(0.0, 1.0);
    for (int i = 0; i < N; ++i) {
        samples[i] = distribution(generator);
    }
    return samples;
}
inline double d1(double S, double K, double r, double sigma, double T,
                 double yield = 0.0) {
    return (std::log(S / K) + ((r - yield) + 0.5 * sigma * sigma) * T) /
           (sigma * std::sqrt(T));
}
inline double d2(double d1, double sigma, double T) {
    return d1 - sigma * std::sqrt(T);
}
} // namespace utils