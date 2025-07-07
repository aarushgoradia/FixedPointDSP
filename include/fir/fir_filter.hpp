#pragma once

#include <array>
#include <cstddef>
#include "fixed_point/fixed_point.hpp"

namespace dsp {

    template<typename SampleType, std::size_t Taps>
    class FIRFilter {
    public:
        using CoeffArray = std::array<SampleType, Taps>;

        // Constructors
        FIRFilter() {
            reset();
        }

		// Constructor with coefficients
        explicit FIRFilter(const CoeffArray& coeffs) {
			set_coefficients(coeffs);
        }

        // Set coefficients after construction
        void set_coefficients(const CoeffArray& coeffs) {
            coeffs_ = coeffs;
			reset();
        }

        // Process one input sample and return the filtered output
        SampleType process(SampleType input_sample) {
            // Add new sample to the buffer
            buffer_[buffer_index_] = input_sample;

            SampleType output = 0;
            std::size_t tap_index = buffer_index_;

            for (std::size_t i = 0; i < Taps; ++i) {
				// Calculate the filtered output output[n] = sum(c[k] * x[n-k])
                output += coeffs_[i] * buffer_[tap_index];
				// Move to the next tap index, go around if necessary
                tap_index = (tap_index == 0) ? Taps - 1 : tap_index - 1;
			}

			// Move the buffer index forward
			buffer_index_ = (buffer_index_ + 1) % Taps;

            return output;
        }

        // Reset internal buffer/state
        void reset() {
            buffer_.fill(SampleType(0));
			buffer_index_ = 0;
        }

        // Get internal coefficients (helper)
        const CoeffArray& coefficients() const {
            return coeffs_;
        }

    private:
        CoeffArray coeffs_{};
        std::array<SampleType, Taps> buffer_{};
        std::size_t buffer_index_{0};
    };

} // namespace dsp
