#include <gtest/gtest.h>
#include <vector>
#include <complex>

#include "dsp/fft.hpp"
#include "dsp/dft.hpp"
#include "fixed_point/fixed_point.hpp"

using Fixed = FixedPoint<16, 8, SaturationPolicy>;
using CFixed = std::complex<Fixed>;

namespace {

    /// Helper to wrap a real vector into complex samples
    static std::vector<CFixed> to_complex(const std::vector<Fixed>& real) {
        std::vector<CFixed> c(real.size());
        for (size_t i = 0; i < real.size(); ++i)
            c[i] = CFixed(real[i], Fixed{ 0 });
        return c;
    }

    /// Compare two complex-sample vectors elementwise within tol
    static void expect_complex_eq(
        const std::vector<CFixed>& a,
        const std::vector<CFixed>& b,
        float tol = 0.01f)
    {
        ASSERT_EQ(a.size(), b.size());
        for (size_t i = 0; i < a.size(); ++i) {
            EXPECT_NEAR(a[i].real().to_float(), b[i].real().to_float(), tol)
                << "real part mismatch at index " << i;
            EXPECT_NEAR(a[i].imag().to_float(), b[i].imag().to_float(), tol)
                << "imag part mismatch at index " << i;
        }
    }

    TEST(FFTTest, ImpulseFrequencyDomain) {
        // N=8 impulse in time → all-ones spectrum
        const size_t N = 8;
        std::vector<CFixed> data(N, CFixed{ Fixed{0}, Fixed{0} });
        data[0] = CFixed{ Fixed{1}, Fixed{0} };

        dsp::FFTPlan<Fixed> plan(N);
        plan.forward(data);

        // Expect X[k] = 1 + j0 for all k
        for (size_t k = 0; k < N; ++k) {
            EXPECT_NEAR(data[k].real().to_float(), 1.0f, 0.01f) << "k=" << k;
            EXPECT_NEAR(data[k].imag().to_float(), 0.0f, 0.01f) << "k=" << k;
        }
    }

    TEST(FFTTest, RoundTripRealSignal) {
        // N=8 arbitrary real signal
        const size_t N = 8;
        std::vector<Fixed> real{ Fixed{1.0f}, Fixed{0.5f}, Fixed{-1.0f}, Fixed{2.25f},
                                Fixed{0.0f}, Fixed{-0.75f}, Fixed{1.5f}, Fixed{-0.25f} };

        // to complex, forward FFT, inverse FFT
        auto data = to_complex(real);
        dsp::FFTPlan<Fixed> plan(N);
        plan.forward(data);
        plan.inverse(data);

        // After inverse, imag parts ≈ 0 and real parts ≈ original
        ASSERT_EQ(data.size(), N);
        for (size_t i = 0; i < N; ++i) {
            EXPECT_NEAR(data[i].real().to_float(), real[i].to_float(), 0.02f)
                << "real mismatch at " << i;
            EXPECT_NEAR(data[i].imag().to_float(), 0.0f, 0.02f)
                << "imag mismatch at " << i;
        }
    }

    TEST(FFTTest, MatchesNaiveDFT) {
        // N=4 simple sequence
        const size_t N = 4;
        std::vector<Fixed> real{ Fixed{1.0f}, Fixed{2.0f}, Fixed{3.0f}, Fixed{4.0f} };
        auto x_c = to_complex(real);

        // compute naive DFT
        auto X_dft = dsp::dft(real);

        // compute FFTPlan
        dsp::FFTPlan<Fixed> plan(N);
        auto X_fft = x_c;
        plan.forward(X_fft);

        // compare
        expect_complex_eq(X_fft, X_dft, 0.02f);
    }

}  // namespace
