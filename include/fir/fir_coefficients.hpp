#pragma once

#include <vector>
#include <cmath>
#include <numeric>
#include <numbers>
#include <stdexcept>
#include <algorithm>
#include "fir_filter.hpp"

namespace dsp {

    // Enum to define filter types (optional if you want to support switch-style API)
    enum class FilterType {
        LowPass,
        HighPass
    };

	// Specific helper for low-pass filter generation
    template<typename SampleType>
    std::vector<SampleType> generate_lowpass_coefficients(std::size_t num_taps,
        double sample_rate,
        double cutoff_freq) {

        std::vector<SampleType> coefficients(num_taps);
        double nyquist = sample_rate / 2.0;
        double normalized_cutoff = cutoff_freq / nyquist;

        for (std::size_t n = 0; n < num_taps; ++n) {
            int centered_n = static_cast<int>(n) - static_cast<int>((num_taps - 1) / 2);

            if (centered_n == 0) {
                coefficients[n] = static_cast<SampleType>(2 * normalized_cutoff);
            }
            else {
                double x = static_cast<double>(centered_n);
                coefficients[n] = static_cast<SampleType>(
                    std::sin(std::numbers::pi * 2 * normalized_cutoff * x) / (std::numbers::pi * x)
                    );
            }
        }

        // Apply Hamming window
        for (std::size_t n = 0; n < num_taps; ++n) {
            coefficients[n] *= static_cast<SampleType>(
                0.54 - 0.46 * std::cos(2 * std::numbers::pi * n / (num_taps - 1))
                );
        }

        // Normalize
        SampleType sum = std::accumulate(coefficients.begin(), coefficients.end(), SampleType(0));
        for (auto& coeff : coefficients) {
            coeff /= sum;
        }

        return coefficients;
    }

    // Specific helper for high-pass filter generation
    template<typename SampleType>
    std::vector<SampleType> generate_highpass_coefficients(std::size_t num_taps,
        double sample_rate,
        double cutoff_freq) {

        std::vector<SampleType> coefficients(num_taps);
        double nyquist = sample_rate / 2.0;
        double normalized_cutoff = cutoff_freq / nyquist;

        for (std::size_t n = 0; n < num_taps; ++n) {
            int centered_n = static_cast<int>(n) - static_cast<int>((num_taps - 1) / 2);

            if (centered_n == 0) {
                coefficients[n] = static_cast<SampleType>(1 - 2 * normalized_cutoff);
            }
            else {
                double x = static_cast<double>(centered_n);
                coefficients[n] = static_cast<SampleType>(
                    -std::sin(std::numbers::pi * 2 * normalized_cutoff * x) / (std::numbers::pi * x)
                    );
            }
        }

        // Apply Hamming window
        for (std::size_t n = 0; n < num_taps; ++n) {
            coefficients[n] *= static_cast<SampleType>(
                0.54 - 0.46 * std::cos(2 * std::numbers::pi * n / (num_taps - 1))
                );
        }

        return coefficients;
    }


    template<typename SampleType, std::size_t Taps>
    FIRFilter<SampleType, Taps> make_lowpass_filter(double sample_rate, double cutoff_freq) {
        // 1) generate a vector of length Taps
        auto vec = generate_lowpass_coefficients<SampleType>(Taps, sample_rate, cutoff_freq);

        // 2) copy into a std::array<SampleType,Taps>
        std::array<SampleType, Taps> arr{};
        std::copy_n(vec.begin(), Taps, arr.begin());

        // 3) now call the FIRFilter constructor that takes an array
        return FIRFilter<SampleType, Taps>(arr);
    }

    template<typename SampleType, std::size_t Taps>
    FIRFilter<SampleType, Taps> make_highpass_filter(double sample_rate, double cutoff_freq) {
        auto vec = generate_highpass_coefficients<SampleType>(Taps, sample_rate, cutoff_freq);
        std::array<SampleType, Taps> arr{};
        std::copy_n(vec.begin(), Taps, arr.begin());
        return FIRFilter<SampleType, Taps>(arr);
    }

    // General coefficient generator for any FIR filter
    template<typename SampleType>
    std::vector<SampleType> generate_fir_coefficients(FilterType type,
                                                      std::size_t num_taps,
                                                      double sample_rate,
                                                      double cutoff_freq,
                                                      double cutoff_freq2 = 0.0) {
        switch (type) {
            case FilterType::LowPass:
                return generate_lowpass_coefficients<SampleType>(num_taps, sample_rate, cutoff_freq);
            case FilterType::HighPass:
                return generate_highpass_coefficients<SampleType>(num_taps, sample_rate, cutoff_freq);
            default:
                throw std::invalid_argument("Unsupported filter type");
        }
    }

} // namespace dsp
