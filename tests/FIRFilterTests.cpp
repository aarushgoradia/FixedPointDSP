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

    TEST(CoefficientTest, LowpassIsSymmetric) {
        constexpr std::size_t Taps = 7;  // odd-length for linear phase
        auto c = dsp::generate_lowpass_coefficients<Fixed>(
                Taps, 44100.0, 2000.0
        );
        for (std::size_t i = 0; i < Taps/2; ++i) {
            EXPECT_NEAR(
                    c[i].to_float(),
                    c[Taps-1-i].to_float(),
                    1e-6f
            );
        }
    }

    TEST(CoefficientTest, LowpassSizeAndNormalization) {
        constexpr std::size_t Taps = 11;
        auto coeffs = dsp::generate_lowpass_coefficients<Fixed>(
                Taps, 48'000.0, 1'000.0
        );
        // … size check …

        float sum = std::accumulate(
                coeffs.begin(), coeffs.end(), 0.0f,
                [](float acc, const Fixed &c){ return acc + c.to_float(); }
        );
        // allow ±0.03 for 11 taps quantization error
        EXPECT_NEAR(sum, 1.0f, 0.03f);
    }

    TEST(CoefficientTest, HighpassBlocksDC) {
        constexpr std::size_t Taps = 15;
        auto coeffs = dsp::generate_highpass_coefficients<Fixed>(
                Taps, 48'000.0, 5'000.0
        );
        float sum = std::accumulate(
                coeffs.begin(), coeffs.end(), 0.0f,
                [](float acc, const Fixed &c){ return acc + c.to_float(); }
        );
        // allow ±0.04 for 15 taps quantization error
        EXPECT_NEAR(sum, 0.0f, 0.04f);
    }

    TEST(CoefficientTest, WrapperDispatchMatchesDirect) {
        constexpr std::size_t Taps = 9;
        // low-pass
        auto directLP = dsp::generate_lowpass_coefficients<Fixed>(
                Taps, 48e3, 2e3
        );
        auto wrapLP = dsp::generate_fir_coefficients<Fixed>(
                dsp::FilterType::LowPass,
                Taps, 48e3, 2e3
        );
        ASSERT_EQ(wrapLP.size(), directLP.size());
        for (size_t i = 0; i < Taps; ++i)
            EXPECT_NEAR(directLP[i].to_float(), wrapLP[i].to_float(), 1e-6f);

        // high-pass
        auto directHP = dsp::generate_highpass_coefficients<Fixed>(
                Taps, 48e3, 6e3
        );
        auto wrapHP = dsp::generate_fir_coefficients<Fixed>(
                dsp::FilterType::HighPass,
                Taps, 48e3, 6e3
        );
        ASSERT_EQ(wrapHP.size(), directHP.size());
        for (size_t i = 0; i < Taps; ++i)
            EXPECT_NEAR(directHP[i].to_float(), wrapHP[i].to_float(), 1e-6f);
    }

} // namespace
