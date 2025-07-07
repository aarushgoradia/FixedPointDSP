# FixedPointDSP

**FixedPointDSP** is a modern, header-only C++23 library for fixed-point arithmetic and basic digital signal processing (DSP) components. It is designed for embedded, simulation, and hardware-aligned applications where floating-point math is either too expensive or unavailable.

> 🚧 This project is under active development by [@aarushgoradia](https://github.com/aarushgoradia)

---

## ✨ Features

- 📦 Header-only design — no linking required
- 🧮 Custom `FixedPoint<TotalBits, FractionBits, OverflowPolicy>` number type
- ➕ Arithmetic operator support with compile-time correctness
- 🧪 Unit tested using Google Test
- 🎚️ FIR filter and basic DSP modules (convolution, DFT))
- 🧰 Cross-platform via CMake — builds on Windows, macOS, Linux

---

## 📁 Project Structure
```
include/
├── fixed_point/
│ ├── fixed_point.hpp → Core FixedPoint class
│ ├── arithmetic_policies.hpp → Overflow policies
│ ├── promote.hpp → Promotion rules for arithmetic
├── filters/
│ └── fir_filter_.hpp → FIR filter implementation
| └── fir_coefficients_.hp → FIR filter implementation
└-- dsp/
  ├── convolution.hpp → Convolution operation
  └── dft.hpp → Discrete Fourier Transform (DFT) module (TODO)

tests/ → Unit tests
├── FixedPointTests.cpp
└── FIRFilterTests.cpp
└── ConvolutionTests.cpp
```
---

## 🛠️ Getting Started

### ✅ Requirements

- C++23-compliant compiler (GCC 13+, Clang 16+, MSVC 2022+)
- CMake ≥ 3.20
- (Optional) Google Test for unit testing

### 🔧 Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
``` 

### 🧪 Run Tests

```bash
./build/FixedPointTests
```

---

### 🎯 Roadmap
- [x] Project scaffold with CMake + GitHub

- [x] Cross-platform structure and .gitignore cleanup

- [x] Implement FixedPoint core class

- [x] Add operator overloads and conversion functions

- [x] Set up Google Test framework

- [x] Write test coverage for all constructors & operations

- [X] Write saturation overflow policies

- [X] Write tests for overflow policies

- [X] Add FIR filter class and verify sample responses

- [X] Add and test convolution module

- [ ] Add DFT and FFT modules, and test

---

### 🧠 Why Fixed-Point?
Fixed-point arithmetic is essential in:

- Embedded systems (where float is unavailable or too slow)

- DSP hardware blocks (e.g., Verilog models, FPGAs)

- Systems where deterministic, bounded precision is needed

- This library simulates those behaviors in modern C++, useful for both software prototyping and hardware-aligned simulation.

---

### 📜 License
This project is released under the MIT License.
Feel free to fork, modify, and build upon it.

---

### 👋 Author
Aarush Goradia
Electrical & Computer Engineering student at Princeton
Exploring hardware, C++, and systems design
https://github.com/aarushgoradia