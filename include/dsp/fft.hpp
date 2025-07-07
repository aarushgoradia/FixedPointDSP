// File: include/dsp/fft.hpp
#pragma once

// Silence MSVC’s non-floating std::complex warning
#ifndef _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING
#define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING
#endif

#include <vector>
#include <complex>
#include <concepts>
#include <cstddef>
#include <stdexcept>
#include "dft.hpp"
#include "concepts.hpp"

namespace dsp {

    // Alias for a complex sample
    template<Arithmetic SampleType>
    using complex_sample = std::complex<SampleType>;

    // Plan for an in‐place radix-2 FFT of length N (power of two)
    template<Arithmetic SampleType>
    struct FFTPlan {
        std::size_t N;                                     ///< transform size
        std::vector<std::size_t> bitrev;                   ///< bit-reversed indices
        std::vector<complex_sample<SampleType>> twiddles;  ///< W_N^k = exp(−2*pi*i*k/N)

        // Build tables for size N (must be a power of two)
        explicit FFTPlan(std::size_t N) {
            // Check power‐of‐two, this is required for FFT
            if (N == 0 || (N & (N - 1)) != 0) {
                throw std::invalid_argument("FFT size must be a power of two");
            }
			this->N = N;

            // Compute log2(N) as an integer number of bits
            std::size_t levels = 0;
            while ((std::size_t(1) << levels) < N) ++levels;

            // Build the bit-reversal table
            bitrev.resize(N);
            for (std::size_t i = 0; i < N; ++i) {
                std::size_t r = 0;
                for (std::size_t j = 0; j < levels; ++j) {
                    // extract bit j of i, put it at position (levels-1-j)
                    if (i & (std::size_t(1) << j)) {
                        r |= (std::size_t(1) << (levels - 1 - j));
                    }
                }
                bitrev[i] = r;
            }

            // Precompute forward twiddles W_N^k for k=0..N/2−1
            twiddles.resize(N / 2);
            for (std::size_t k = 0; k < N / 2; ++k) {
                twiddles[k] = make_twiddle<SampleType>(N, k, /*inverse=*/false);
            }
        }

        // In‐place forward FFT (no 1/N scaling)
        void forward(std::vector<complex_sample<SampleType>>& data) const {
            if (data.size() != N) {
				throw std::invalid_argument("Data size must match FFT plan size");
            }

            // Bit reverse the input data
            for (std::size_t i = 0; i < N; ++i) {
                if (i < bitrev[i]) {
                    std::swap(data[i], data[bitrev[i]]);
                }
            }

            // Perform the iterative butterfly FFT algorithm
            for (std::size_t len = 2; len <= N; len <<= 1) {
                auto half = len >> 1;
				auto step = N / len;
                for (std::size_t i = 0; i < N; i += len) {
                    for (std::size_t j = 0; j < half; ++j) {
                        auto u = data[i + j];
                        auto v = data[i + j + half] * twiddles[j * step];
                        data[i + j] = u + v;
                        data[i + j + half] = u - v;
                    }
				}
            }
        }

        // In‐place inverse FFT (with 1/N scaling)
        void inverse(std::vector<complex_sample<SampleType>>& data) const {
            if (data.size() != N) {
                throw std::invalid_argument("Data size must match FFT plan size");
            }

            // Conjugate the input for inverse FFT
			for (auto& x : data) x = std::conj(x);  

            // Reuse the forward FFT
			forward(data); 

            // Conjugate back to get the correct inverse
			for (auto& x : data) x = std::conj(x);

            // Scale by 1/N
			SampleType invN = SampleType(1.0) / SampleType(double(N));
			for (auto& x : data) x *= invN; 
        }
    };

} // namespace dsp
