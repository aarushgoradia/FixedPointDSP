#pragma once

#include <iostream>
#include <cstdint>
#include <type_traits>
#include <cmath>
#include <stdexcept>
#include "arithmetic_policies.hpp"


// ----------------- Fixed Point Class Template -----------------
template<int TotalBits, int FractionalBits, template<typename, template<typename> class, int> class OverFlowPolicy = WrapAroundPolicy>
class FixedPoint {
	static_assert(TotalBits > 0, "Total bits must be positive");
	static_assert(FractionalBits >= 0 && FractionalBits < TotalBits, "Fractional bits must be valid");

private:
	static constexpr auto select_storage_type() {
		if constexpr (TotalBits <= 8) {
			return int8_t{};
		}
		else if constexpr (TotalBits <= 16) {
			return int16_t{};
		}
		else if constexpr (TotalBits <= 32) {
			return int32_t{};
		}
		else {
			return int64_t{};
		}
	}

public:
    using StorageType = decltype(select_storage_type());

private:
    StorageType value;
    using Policy = OverFlowPolicy<StorageType, Promote, FractionalBits>;

public:
	// -----------------Constructors-----------------
	
	// Default constructor initializes to zero
	FixedPoint() {
		value = 0;
	}
	
	// Constructor from integer
	FixedPoint(int integer) {
		if constexpr (FractionalBits == 0) {
			value = static_cast<StorageType>(integer);
		} else {
			value = static_cast<StorageType>(integer) << FractionalBits;
			
		}
	}

	// Constructor from floating-point number
	FixedPoint(float number) {
		value = static_cast<StorageType>(std::round(number * (1 << FractionalBits)));
	}

	// Constructor from double
	FixedPoint(double number) {
		value = static_cast<StorageType>(std::round(number * (1 << FractionalBits)));
	}

	// Constructor from raw storage type
	static FixedPoint from_raw(StorageType v) {
		FixedPoint fp;
		fp.value = v;
		return fp;
	}

	// ------------------Conversion Operators-----------------

	// Conversion to integer
	int to_int() const {
		if constexpr (FractionalBits == 0) {
			return static_cast<int>(value);
		} else {
			return static_cast<int>(value >> FractionalBits);
		}
	}

	// Conversion to floating-point number
	float to_float() const {
		return static_cast<float>(value) / (1 << FractionalBits);
	}

	// Conversion to double
	double to_double() const {
		return static_cast<double>(value) / (1 << FractionalBits);
	}

	StorageType raw() const {
		return value;
	}

	// -----------------Arithmetic Operators-----------------

	// Unary minus operator
	FixedPoint operator-() const {
		FixedPoint zero{ 0 };
		return zero - *this;
	}

	// Addition, subtraction, multiplication, and division
	FixedPoint operator+(const FixedPoint& other) const {
		FixedPoint result;
		result.value = Policy::add(this->value, other.value);
		return result;
	}

	FixedPoint operator-(const FixedPoint& other) const {
		FixedPoint result;
		result.value = Policy::sub(this->value, other.value);
		return result;
	}

	FixedPoint operator*(const FixedPoint& other) const {
		FixedPoint result;
		result.value = Policy::mul(this->value, other.value);
		return result;
	}

	FixedPoint operator/(const FixedPoint& other) const {
		if (other.value == 0) {
			throw std::runtime_error("Division by zero, NAN");
		}
		FixedPoint result;
		result.value = Policy::div(this->value, other.value);
		return result;
	}

	// Compound assignment operators
	FixedPoint& operator+=(const FixedPoint& other) {
		value = Policy::add(value, other.value);
		return *this;
	}

	FixedPoint& operator-=(const FixedPoint& other) {
		value = Policy::sub(value, other.value);
		return *this;
	}

	FixedPoint& operator*=(const FixedPoint& other) {
		value = Policy::mul(value, other.value);
		return *this;
	}

	FixedPoint& operator/=(const FixedPoint& other) {
		if (other.value == 0) {
			throw std::runtime_error("Division by zero, NAN");
		}
		value = Policy::div(value, other.value);
		return *this;
	}

	// -----------------Comparison Operators-----------------

	bool operator==(const FixedPoint& other) const {
		return this->value == other.value;
	}

	bool operator!=(const FixedPoint& other) const {
		return !(*this == other);
	}

	bool operator<(const FixedPoint& other) const {
		return this->value < other.value;
	}

	bool operator<=(const FixedPoint& other) const {
		return this->value <= other.value;
	}

	bool operator>(const FixedPoint& other) const {
		return this->value > other.value;
	}

	bool operator>=(const FixedPoint& other) const {
		return this->value >= other.value;
	}

	// -----------------Stream Output (Debugging)-----------------

	friend std::ostream& operator<<(std::ostream& os, const FixedPoint& fp) {
		return os << fp.to_float();
	}
};
