# FixedPointDSP

A header-only C++23 library for fixed-point math and the DSP blocks built on it: FIR filters, convolution, the DFT and the FFT. It's for when floats are too expensive or unavailable, like on small embedded targets, or when you want a software model that matches the bit widths of real hardware.

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/q-format-dark.svg">
  <img alt="FixedPoint<16, 8> bit layout: one sign bit, seven integer bits, eight fraction bits, holding 1.5" src="docs/q-format-light.svg">
</picture>

`FixedPoint<T, F>` is a `T`-bit signed integer with the binary point `F` bits from the right, so a raw value $q$ means

```math
x = \frac{q}{2^{F}}, \qquad x \in \left[-2^{\,T-F-1},\ 2^{\,T-F-1} - 2^{-F}\right]
```

For `FixedPoint<16, 8>` that's −128 to 127.996, in steps of 1/256. Storage is the smallest of `int8_t` / `int16_t` / `int32_t` / `int64_t` that fits, chosen at compile time.

## Features

- **Any width:** `FixedPoint<TotalBits, FracBits, OverflowPolicy>`
- **Overflow is a policy:** `WrapAroundPolicy` (the default, like hardware) or `SaturationPolicy` (clamps to min/max)
- **Operators:** `+ - * /` and their compound versions, with widening for multiply and divide so intermediates don't overflow
- **DSP:** `FIRFilter`, linear and circular convolution, an O(n²) DFT, and an O(n log n) radix-2 FFT (`FFTPlan`, with precomputed twiddle factors)
- **Tested:** GoogleTest suites for the number type, FIR, convolution, DFT and FFT, plus a Google Benchmark

## Quick look

```cpp
#include "fixed_point/fixed_point.hpp"
#include "fir/fir_filter.hpp"
#include "dsp/fft.hpp"

using Q8_8 = FixedPoint<16, 8, SaturationPolicy>;

Q8_8 a(1.5f), b(2.25f);
Q8_8 c = a * b;              // 3.375
c.raw();                     // 864, the int16_t underneath
c.to_float();                // 3.375f

std::array<Q8_8, 5> taps = {Q8_8(0.1f), Q8_8(0.2f), Q8_8(0.3f), Q8_8(0.2f), Q8_8(0.1f)};
dsp::FIRFilter<Q8_8, 5> lowpass(taps);
Q8_8 y = lowpass.process(Q8_8(1.0f));

std::vector<std::complex<Q8_8>> x(256);
dsp::FFTPlan<Q8_8> plan(x.size());
plan.forward(x);             // in place
plan.inverse(x);             // in place, scaled by 1/n
```

## Performance

FFT vs. the naive DFT, `FixedPoint<16, 8>` with saturation, on a 2024 Intel i7:

| n | DFT | FFT | Speedup |
| ---: | ---: | ---: | ---: |
| 128 | 0.527 ms | 0.008 ms | ~66× |
| 256 | 2.01 ms | 0.018 ms | ~112× |
| 512 | 8.23 ms | 0.041 ms | ~201× |
| 1024 | 42.6 ms | 0.092 ms | ~463× |

## Building and testing

Requires a C++23 compiler (GCC 13+, Clang 16+ or MSVC 2022+) and CMake 3.20+. The tests expect GoogleTest and Google Benchmark in `external/`:

```bash
mkdir -p external
git clone --depth 1 https://github.com/google/googletest external/googletest
git clone --depth 1 https://github.com/google/benchmark external/benchmark

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/FFTBenchmark --benchmark_time_unit=ms
```

To use only the library, add `include/` to your include path. There's nothing to link.

## Repository layout

| Path | Contents |
| --- | --- |
| `include/fixed_point/` | `FixedPoint`, the overflow policies, type promotion |
| `include/fir/` | FIR filter + coefficient helpers |
| `include/dsp/` | Convolution, DFT, FFT |
| `tests/` | GoogleTest suites |
| `benchmarks/` | DFT vs. FFT benchmark |

MIT License.
