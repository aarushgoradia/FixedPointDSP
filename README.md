# FixedPointDSP

**FixedPointDSP** is a modern, header-only C++23 library providing  
high-performance fixed-point arithmetic and essential DSP building-blocks.  
It’s ideal for embedded targets, hardware-aligned simulation, and any context  
where floating-point is too heavy or unavailable.

> by [@aarushgoradia](https://github.com/aarushgoradia)

---

## Highlights

- **Header-only** (just include—no linking or external dependencies)
- **Templated** `FixedPoint<TotalBits, FracBits, OverflowPolicy>`
  - Arbitrary bit-width, configurable overflow (wrapping, saturation)
- **Operator overloads** with compile-time safety (C++23 concepts)
- **DSP primitives**:
  - FIR filter  
  - Convolution (linear & circular)  
  - **DFT** & **FFT**  
- **Unit-tested** with Google Test and benchmarked with Google Benchmark
- **Cross-platform** via CMake (Windows, Linux, macOS)

---

## Project Layout
```
├── include/
│ ├── fixed_point/
│ │ ├── fixed_point.hpp # core class
│ │ ├── arithmetic_policies.hpp # overflow rules
│ │ └── promote.hpp # promotion logic
│ ├── fir/ # FIR filter headers
│ │ ├── fir_filter.hpp # FIR filter implementation
│ │ └── fir_coefficients.hpp # coefficient helpers
│ └── dsp/
│ ├── convolution.hpp # linear & circular conv.
│ ├── dft.hpp # O(N²) DFT
│ └── fft.hpp # O(N log N) FFT
├── tests/ # Google Test unit-tests
│ ├── FixedPointTests.cpp
│ ├── FIRFilterTests.cpp
│ ├── ConvolutionTests.cpp
│ ├── DFTTests.cpp
│ └── FFTTests.cpp
├── benchmarks/ # Google Benchmark performance tests
│ └── fft_benchmark.cpp
├── external/ # third-party (googletest, benchmark)
├── CMakeLists.txt
└── README.md
```

---

## Getting Started

### Prerequisites

- **Compiler**: C++23 support (GCC 13+, Clang 16+, MSVC 2022+)  
- **Build**: CMake ≥ 3.20  
- **(Optional)** Google Test & Google Benchmark (included as `external/`)

### Build & Test

1. Configure & build
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

2. Run unit tests
```bash
ctest --output-on-failure
```

3. (Optional) Run FFT benchmark
```bash
./build/FFTBenchmark --benchmark_time_unit=ms
```
Performance
```
 ------------------------------------------------------
Benchmark            Time             CPU   Iterations
------------------------------------------------------
BM_DFT/128       0.527 ms        0.516 ms         1120
BM_DFT/256        2.01 ms         2.04 ms          345
BM_DFT/512        8.23 ms         8.33 ms           90
BM_DFT/1024       42.6 ms         42.5 ms           18
BM_FFT/128       0.008 ms        0.008 ms        89600
BM_FFT/256       0.018 ms        0.018 ms        37333
BM_FFT/512       0.041 ms        0.040 ms        17231
BM_FFT/1024      0.092 ms        0.092 ms         7467
```

Measured on a 2024 Intel i7, FixedPoint <16,8> saturation policy.

---

## Roadmap
- [x] Core FixedPoint class & arithmetic

- [x] Overflow policies (wrap, saturate)

- [x] FIR filter & convolution + unit tests

- [x] DFT & unit tests

- [x] FFT, unit tests & performance benchmarks

---

## 🙋 Author
Aarush Goradia
B.S.E. in ECE, Princeton University — building C++, hardware, and systems.
