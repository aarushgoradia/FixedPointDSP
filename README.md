# FixedPointDSP

**FixedPointDSP** is a modern, header-only C++23 library for fixed-point arithmetic and basic digital signal processing (DSP) components. It is designed for embedded, simulation, and hardware-aligned applications where floating-point math is either too expensive or unavailable.

> 🚧 This project is under active development by [@aarushgoradia](https://github.com/aarushgoradia) as a deep dive into C++ templates, constexpr math, DSP principles, and clean software architecture.

---

## ✨ Features

- 📦 Header-only design — no linking required
- 🧮 Custom `FixedPoint<TotalBits, FractionBits>` number type
- ➕ Arithmetic operator support with compile-time correctness
- 🧪 Unit tested using Google Test (planned)
- 🎚️ FIR filter and basic DSP modules (WIP)
- 🧰 Cross-platform via CMake — builds on Windows, macOS, Linux

---

## 📁 Project Structure
```
include/fixedpoint/ → Core headers
├── fixed_point.hpp → FixedPoint number class
├── filters/
│ └── fir.hpp → FIR filter implementation (WIP)
└── math/
└── dft.hpp → DFT or convolution helpers (future)

tests/ → Unit tests
├── test_fixed_point.cpp
└── test_fir.cpp
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

- [ ] Write saturation overflow policies

- [ ] Write tests for overflow policies

- [ ] Add FIR filter class and verify sample responses

- [ ] Add DFT and convolution module

---

### 🧠 Why Fixed-Point?
Fixed-point arithmetic is essential in:

- Embedded systems (where float is unavailable or too slow)

- DSP hardware blocks (e.g., Verilog models, FPGAs)

- Systems where deterministic, bounded precision is needed

- This library simulates those behaviors in modern C++, useful for both software prototyping and hardware-aligned simulation.

### 📜 License
This project is released under the MIT License.
Feel free to fork, modify, and build upon it.

### 👋 Author
Aarush Goradia
Electrical & Computer Engineering student at Princeton
Exploring hardware, C++, and systems design
https://github.com/aarushgoradia