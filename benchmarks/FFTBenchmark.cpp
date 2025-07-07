#include <benchmark/benchmark.h>
#include "dsp/dft.hpp"
#include "dsp/fft.hpp"
#include "fixed_point/fixed_point.hpp"

using Fixed = FixedPoint<16, 8, SaturationPolicy>;
using CFixed = std::complex<Fixed>;

static void BM_DFT(benchmark::State& st) {
    size_t N = st.range(0);
    std::vector<CFixed> data(N);
    // fill data…
    for (auto _ : st) {
        benchmark::DoNotOptimize(dsp::dft(data));
    }
}
BENCHMARK(BM_DFT)->Arg(128)->Arg(256)->Arg(512)->Arg(1024);

static void BM_FFT(benchmark::State& st) {
    size_t N = st.range(0);
    std::vector<CFixed> data(N);
    dsp::FFTPlan<Fixed> plan(N);
    for (auto _ : st) {
        auto tmp = data;
        plan.forward(tmp);
        benchmark::DoNotOptimize(tmp);
    }
}
BENCHMARK(BM_FFT)->Arg(128)->Arg(256)->Arg(512)->Arg(1024);

BENCHMARK_MAIN();
