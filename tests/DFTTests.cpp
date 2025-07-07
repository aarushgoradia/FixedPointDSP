#include <gtest/gtest.h>
#include <vector>
#include <complex>
#include <numbers>

#include "dsp/dft.hpp"
#include "fixed_point/fixed_point.hpp"

using Fixed = FixedPoint<16, 8, SaturationPolicy>;
using CFixed = std::complex<Fixed>;

namespace {

    TEST(DFTTest, RealImpulse) {
        // N = 4 impulse in time domain → all-ones spectrum
        std::vector<Fixed> signal = { Fixed(1.0f), Fixed(0.0f), Fixed(0.0f), Fixed(0.0f) };
        auto X = dsp::dft(signal);
        ASSERT_EQ(X.size(), 4u);
        for (std::size_t k = 0; k < X.size(); ++k) {
            EXPECT_NEAR(X[k].real().to_float(), 1.0f, 0.01f);
            EXPECT_NEAR(X[k].imag().to_float(), 0.0f, 0.01f);
        }
    }

    TEST(DFTTest, ComplexKnownSequence) {
        // time-domain: [1,2,3,4]
        // expected DFT (N=4):
        // k=0:  1+2+3+4 = 10
        // k=1:  1 - j2 -3 + j4 = -2 + 2j
        // k=2:  1 -2 +3 -4       = -2 + 0j
        // k=3:  1 + j2 -3 - j4   = -2 - 2j
        std::vector<CFixed> signal = {
            { Fixed(1.0f), Fixed(0.0f) },
            { Fixed(2.0f), Fixed(0.0f) },
            { Fixed(3.0f), Fixed(0.0f) },
            { Fixed(4.0f), Fixed(0.0f) }
        };

        auto X = dsp::dft(signal);
        ASSERT_EQ(X.size(), 4u);

        struct { float re, im; } expected[] = {
            { 10.0f,  0.0f },
            { -2.0f,  2.0f },
            { -2.0f,  0.0f },
            { -2.0f, -2.0f }
        };

        for (std::size_t k = 0; k < X.size(); ++k) {
            EXPECT_NEAR(X[k].real().to_float(), expected[k].re, 0.01f);
            EXPECT_NEAR(X[k].imag().to_float(), expected[k].im, 0.01f);
        }
    }

    TEST(DFTTest, RoundTripRealSequence) {
        // Random real sequence
        std::vector<Fixed> signal = { Fixed(1.0f), Fixed(0.5f), Fixed(-2.0f), Fixed(3.25f) };

        // Forward DFT → inverse DFT
        auto X = dsp::dft(signal);
        auto x2 = dsp::idft(X);

        ASSERT_EQ(x2.size(), signal.size());
        for (std::size_t n = 0; n < signal.size(); ++n) {
            // Imaginary part should be ~0
            EXPECT_NEAR(x2[n].imag().to_float(), 0.0f, 0.02f);
            // Real part should match original
            EXPECT_NEAR(x2[n].real().to_float(), signal[n].to_float(), 0.02f);
        }
    }

}  // namespace
