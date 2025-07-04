#include <gtest/gtest.h>
#include "fir/fir_filter.hpp"
#include "fir/fir_coefficients.hpp"
#include "fixed_point/fixed_point.hpp"

using Fixed = FixedPoint<16, 8>;

namespace {

    TEST(FIRFilterTest, ImpulseResponseReturnsCoefficients) {
        constexpr std::size_t Taps = 5;
        std::array<Fixed, Taps> coeffs = {
            Fixed(0.1f), Fixed(0.2f), Fixed(0.3f), Fixed(0.2f), Fixed(0.1f)
        };
        dsp::FIRFilter<Fixed, Taps> filter(coeffs);

        std::vector<Fixed> outputs;
        outputs.push_back(filter.process(Fixed(1.0f)));  // Impulse
        for (int i = 1; i < Taps; ++i)
            outputs.push_back(filter.process(Fixed(0.0f)));

        for (std::size_t i = 0; i < Taps; ++i) {
            EXPECT_NEAR(outputs[i].to_float(), coeffs[i].to_float(), 0.01);
        }
    }

    TEST(FIRFilterTest, ConstantSignalConvergesToInput) {
        constexpr std::size_t Taps = 3;
        std::array<Fixed, Taps> coeffs = {
            Fixed(1.0f / 3), Fixed(1.0f / 3), Fixed(1.0f / 3)
        };
        dsp::FIRFilter<Fixed, Taps> filter(coeffs);

        Fixed input(3.0f);
        Fixed output;
        for (int i = 0; i < 10; ++i) {
            output = filter.process(input);
        }

        EXPECT_NEAR(output.to_float(), 3.0f, 0.05);
    }

    TEST(FIRFilterTest, ResetClearsBuffer) {
        constexpr std::size_t Taps = 4;
        std::array<Fixed, Taps> coeffs = {
            Fixed(0.25f), Fixed(0.25f), Fixed(0.25f), Fixed(0.25f)
        };
        dsp::FIRFilter<Fixed, Taps> filter(coeffs);

        filter.process(Fixed(1.0f));
        filter.reset();

        auto output = filter.process(Fixed(0.0f));
        EXPECT_NEAR(output.to_float(), 0.0f, 0.001);
    }

    TEST(FIRFilterTest, MakeLowpassFilterWorks) {
        constexpr std::size_t Taps = 11;
        auto filter = dsp::make_lowpass_filter<Fixed, Taps>(48000.0, 1000.0);
        auto out = filter.process(Fixed(1.0f));

        EXPECT_TRUE(std::isfinite(out.to_float()));
    }

    TEST(FIRFilterTest, MakeHighpassFilterWorks) {
        constexpr std::size_t Taps = 15;
        auto filter = dsp::make_highpass_filter<Fixed, Taps>(48000.0, 5000.0);
        auto out = filter.process(Fixed(1.0f));

        EXPECT_TRUE(std::isfinite(out.to_float()));
    }

} // namespace
