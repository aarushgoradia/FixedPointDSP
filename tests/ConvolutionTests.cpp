#include <gtest/gtest.h>
#include "dsp/convolution.hpp"
#include "fixed_point/fixed_point.hpp"

using Fixed = FixedPoint<16, 8, SaturationPolicy>;

namespace {

    TEST(ConvolutionTest, LinearImpulseResponse) {
        std::vector<Fixed> signal = { Fixed(1.0f), Fixed(0.0f), Fixed(0.0f) };
        std::vector<Fixed> kernel = { Fixed(0.2f), Fixed(0.3f), Fixed(0.5f) };

        auto result = dsp::convolve(signal, kernel);

        ASSERT_EQ(result.size(), 5);
        EXPECT_NEAR(result[0].to_float(), 0.2f, 0.01);
        EXPECT_NEAR(result[1].to_float(), 0.3f, 0.01);
        EXPECT_NEAR(result[2].to_float(), 0.5f, 0.01);
        EXPECT_NEAR(result[3].to_float(), 0.0f, 0.01);
        EXPECT_NEAR(result[4].to_float(), 0.0f, 0.01);
    }

    TEST(ConvolutionTest, CircularWrapsCorrectly) {
        std::vector<Fixed> signal = { Fixed(1.0f), Fixed(2.0f), Fixed(3.0f) };
        std::vector<Fixed> kernel = { Fixed(0.5f), Fixed(0.5f), Fixed(0.0f) };

        auto result = dsp::circular_convolve(signal, kernel);

        ASSERT_EQ(result.size(), 3);
        float expected[] = {
            1.0f * 0.5f + 3.0f * 0.5f,  // = 2.0
            2.0f * 0.5f + 1.0f * 0.5f,  // = 1.5
            3.0f * 0.5f + 2.0f * 0.5f   // = 2.5
        };
        for (int i = 0; i < 3; ++i) {
            EXPECT_NEAR(result[i].to_float(), expected[i], 0.01);
        }
    }

    TEST(ConvolutionTest, CorrelateMatchesExpected) {
        std::vector<Fixed> signal = { Fixed(1.0f), Fixed(2.0f), Fixed(3.0f), Fixed(4.0f) };
        std::vector<Fixed> kernel = { Fixed(1.0f), Fixed(0.0f), Fixed(-1.0f) };

        auto result = dsp::correlate(signal, kernel);

        ASSERT_EQ(result.size(), 6);

        // Manually calculated expected correlation (sliding kernel over signal without flipping)
        float expected[] = {
            1.0f * 1.0f,                         // = 1
            2.0f * 1.0f + 1.0f * 0.0f,           // = 2
            3.0f * 1.0f + 2.0f * 0.0f + 1.0f * -1, // = 3 - 1 = 2
            4.0f * 1.0f + 3.0f * 0.0f + 2.0f * -1, // = 4 - 2 = 2
            4.0f * 0.0f + 3.0f * -1,               // = -3
            4.0f * -1.0f                           // = -4
        };

        for (int i = 0; i < 6; ++i) {
            EXPECT_NEAR(result[i].to_float(), expected[i], 0.01);
        }
    }

} // namespace
