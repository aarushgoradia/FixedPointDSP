#pragma once

// Silence the MSVC warning about non‐floating std::complex<T>
#ifndef _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING
#define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING
#endif

#include <vector>
#include <complex>
#include <concepts>
#include <cstddef>
#include <cmath>
#include <numbers>
#include "concepts.hpp"

namespace dsp {
    // Alias for a complex sample
    template<Arithmetic SampleType>
    using complex_sample = std::complex<SampleType>;

    // Helper to build e^{+-2*pi*i*m/N} in SampleType
    template<Arithmetic SampleType>
    complex_sample<SampleType>
        make_twiddle(std::size_t N, std::size_t m, bool inverse = false)
    {
        double sign = inverse ? +1.0 : -1.0;
        double phase = sign * 2.0 * std::numbers::pi * double(m % N) / double(N);
        SampleType re{ std::cos(phase) };
        SampleType im{ std::sin(phase) };
        return complex_sample<SampleType>(re, im);
    }

    // DFT on real‐valued data
    template<Arithmetic SampleType>
    std::vector< complex_sample<SampleType> >
        dft(const std::vector<SampleType>& signal)
    {
        std::size_t N = signal.size();
		// We need this to guarantee that the result vector is initialized to zero
		// This is because the compiler does not recognize how to default initialize a FixedPoint type to (0, 0)
        auto zero_cs = complex_sample<SampleType>(SampleType{ 0 }, SampleType{ 0 });
        std::vector< complex_sample<SampleType> > result(N, zero_cs);

        for (std::size_t k = 0; k < N; ++k) {
            auto sum = complex_sample<SampleType>(SampleType{ 0 }, SampleType{ 0 });
            for (std::size_t n = 0; n < N; ++n) {
                auto W = make_twiddle<SampleType>(N, k * n, /*inverse=*/false);
                auto xcn = complex_sample<SampleType>(signal[n], SampleType{ 0 });
                sum = sum + xcn * W;
            }
            result[k] = sum;
        }

        return result;
    }

    // DFT on complex‐valued data
    template<Arithmetic SampleType>
    std::vector< complex_sample<SampleType> >
        dft(const std::vector< complex_sample<SampleType> >& signal)
    {
        std::size_t N = signal.size();
        auto zero_cs = complex_sample<SampleType>(SampleType{ 0 }, SampleType{ 0 });
        std::vector< complex_sample<SampleType> > result(N, zero_cs);

        for (std::size_t k = 0; k < N; ++k) {
            auto sum = complex_sample<SampleType>(SampleType{ 0 }, SampleType{ 0 });
            for (std::size_t n = 0; n < N; ++n) {
                auto W = make_twiddle<SampleType>(N, k * n, /*inverse=*/false);
                sum = sum + signal[n] * W;
            }
            result[k] = sum;
        }

        return result;
    }

    // Inverse DFT (complex in → complex out, with 1/N scaling)
    template<Arithmetic SampleType>
    std::vector< complex_sample<SampleType> >
        idft(const std::vector< complex_sample<SampleType> >& X)
    {
        std::size_t N = X.size();
        auto zero_cs = complex_sample<SampleType>(SampleType{ 0 }, SampleType{ 0 });
        std::vector< complex_sample<SampleType> > result(N, zero_cs);

        for (std::size_t n = 0; n < N; ++n) {
            auto sum = complex_sample<SampleType>(SampleType{ 0 }, SampleType{ 0 });
            for (std::size_t k = 0; k < N; ++k) {
                auto W = make_twiddle<SampleType>(N, k * n, /*inverse=*/true);
                sum = sum + X[k] * W;
            }
            // scale by 1/N
            result[n] = sum / SampleType{ static_cast<double>(N) };
        }

        return result;
    }

} // namespace dsp
