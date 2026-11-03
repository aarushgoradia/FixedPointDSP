# FixedPointDSP

a header-only c++23 library for fixed-point math and the dsp blocks you build on it: fir filters, convolution, dft and fft. it's for when floats are too expensive or don't exist, like small embedded targets, or when you want a software model that matches the bit widths of real hardware.

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/q-format-dark.svg">
  <img alt="FixedPoint<16, 8> bit layout: one sign bit, seven integer bits, eight fraction bits, holding 1.5" src="docs/q-format-light.svg">
</picture>

`FixedPoint<T, F>` is a `T`-bit signed integer with the binary point `F` bits from the right, so a raw value $q$ means

```math
x = \frac{q}{2^{F}}, \qquad x \in \left[-2^{\,T-F-1},\ 2^{\,T-F-1} - 2^{-F}\right]
```

for `FixedPoint<16, 8>` that's −128 to 127.996, in steps of 1/256. storage is the smallest of `int8_t` / `int16_t` / `int32_t` / `int64_t` that fits, picked at compile time.

## what you get

- **any width:** `FixedPoint<TotalBits, FracBits, OverflowPolicy>`
- **overflow is a policy:** `WrapAroundPolicy` (default, like hardware) or `SaturationPolicy` (clamps to min/max)
- **operators:** `+ - * /` and the compound versions, with widening for multiply and divide so intermediates don't overflow
- **dsp:** `FIRFilter`, linear + circular convolution, an O(n²) dft, and an O(n log n) radix-2 fft (`FFTPlan`, with precomputed twiddles)
- **tested:** google test suites for the number type, fir, convolution, dft and fft, plus a google benchmark

## quick look

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

## speed

fft vs the naive dft, `FixedPoint<16, 8>` with saturation, on a 2024 intel i7:

| n | dft | fft | speedup |
| ---: | ---: | ---: | ---: |
| 128 | 0.527 ms | 0.008 ms | ~66x |
| 256 | 2.01 ms | 0.018 ms | ~112x |
| 512 | 8.23 ms | 0.041 ms | ~201x |
| 1024 | 42.6 ms | 0.092 ms | ~463x |

## build + test

needs a c++23 compiler (gcc 13+, clang 16+, msvc 2022+) and cmake 3.20+. the tests expect googletest and google benchmark in `external/`:

```bash
mkdir -p external
git clone --depth 1 https://github.com/google/googletest external/googletest
git clone --depth 1 https://github.com/google/benchmark external/benchmark

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/FFTBenchmark --benchmark_time_unit=ms
```

to just use the library, add `include/` to your include path. there's nothing to link.

## what's where

| path | what |
| --- | --- |
| `include/fixed_point/` | `FixedPoint`, the overflow policies, type promotion |
| `include/fir/` | fir filter + coefficient helpers |
| `include/dsp/` | convolution, dft, fft |
| `tests/` | google test suites |
| `benchmarks/` | dft vs fft benchmark |

mit license.
