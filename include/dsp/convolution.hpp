#pragma once

#include <vector>
#include <algorithm>

namespace dsp {

	// Linear convolution. Takes in 2 vectors of the same type and returns a vector. Uses standard dot product approach.
    template<typename SampleType>
    std::vector<SampleType> convolve(const std::vector<SampleType>& signal,
        const std::vector<SampleType>& kernel) {
		std::size_t signal_size = signal.size();
		std::size_t kernel_size = kernel.size();
        std::vector<SampleType> result(signal_size + kernel_size - 1, SampleType(0));

		// Perform linear convolution
        for (std::size_t n = 0; n < result.size(); ++n) {
            for (std::size_t k = 0; k < kernel_size; ++k) {
                if (n - k >= 0 && n - k < signal_size) {
                    result[n] += signal[n - k] * kernel[k];
                }
            }
        }
		return result;
    }

	// Circular convolution with same-length output as input. Takes in 2 vectors of the same type and returns a vector.
    template<typename SampleType>
    std::vector<SampleType> circular_convolve(const std::vector<SampleType>& signal,
        const std::vector<SampleType>& kernel) {
		std::size_t signal_size = signal.size();
        std::size_t kernel_size = kernel.size();
        std::size_t result_size = std::max(signal_size, kernel_size);

		auto padded_signal = signal;
		auto padded_kernel = kernel;

		// Pad the signal and kernel to the same size
        if (signal_size < result_size) {
            padded_signal.resize(result_size, SampleType(0));
        }

        if (kernel_size < result_size) {
            padded_kernel.resize(result_size, SampleType(0));
        }

        std::vector<SampleType> result(result_size, SampleType(0));

		// Perform circular convolution
        for (std::size_t n = 0; n < result_size; ++n) {
            SampleType sum = SampleType(0);
            for (std::size_t k = 0; k < result_size; ++k) {
                sum += padded_signal[(n - k + result_size) % result_size] * padded_kernel[k];
            }
			result[n] = sum;
        }

		return result;
    }

	// Cross-correlation. Similar to convolution but without flipping the kernel.
    template<typename SampleType>
    std::vector<SampleType> correlate(const std::vector<SampleType>& signal,
        const std::vector<SampleType>& kernel) {
        std::size_t N = signal.size();
        std::size_t M = kernel.size();
        std::vector<SampleType> result(N + M - 1, SampleType(0));

        // Slide the kernel backwards
        for (std::size_t n = 0; n < result.size(); ++n) {
            for (std::size_t k = 0; k < M; ++k) {
                // only accumulate when n >= k (to avoid underflow)
                // and (n - k) is still in [0 .. N-1]
                if (n >= k && (n - k) < N) {
                    result[n] += signal[n - k] * kernel[k];
                }
            }
        }
        return result;
    }
} // namespace dsp
