#pragma once

#include <iostream>
#include <cstdint>
#include <type_traits>
#include <cmath>

// ----------------- Type Promotion Helper -----------------
template <typename T>
struct Promote { using type = T; };

template <>
struct Promote<int8_t> { using type = int16_t; };

template <>
struct Promote<int16_t> { using type = int32_t; };

template <>
struct Promote<int32_t> { using type = int64_t; };

template <>
struct Promote<int64_t> { using type = int64_t; };


// ----------------- Fixed Point Class Template -----------------
template<int TotalBits, int FractionalBits>
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

	using StorageType = decltype(select_storage_type());

	StorageType value;

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

	// Addition, subtraction, multiplication, and division
	FixedPoint operator+(const FixedPoint& other) const {
		FixedPoint result;
		using PromotedType = typename Promote<StorageType>::type;

		PromotedType this_value = static_cast<PromotedType>(this->value);
		PromotedType other_value = static_cast<PromotedType>(other.value);

		result.value = static_cast<StorageType>(this_value + other_value);
		return result;
	}

	FixedPoint operator-(const FixedPoint& other) const {
		FixedPoint result;
		using PromotedType = typename Promote<StorageType>::type;

		PromotedType this_value = static_cast<PromotedType>(this->value);
		PromotedType other_value = static_cast<PromotedType>(other.value);

		result.value = static_cast<StorageType>(this_value - other_value);
		return result;
	}

	FixedPoint operator*(const FixedPoint& other) const {
		FixedPoint result;
		using PromotedType = typename Promote<StorageType>::type;

		PromotedType this_value = static_cast<PromotedType>(this->value);
		PromotedType other_value = static_cast<PromotedType>(other.value);

		// Multiply and then shift right by FractionalBits to adjust for fixed-point
		result.value = static_cast<StorageType>((this_value * other_value) >> FractionalBits);
		return result;
	}

	FixedPoint operator/(const FixedPoint& other) const {
		FixedPoint result;

		// Divide and then shift left by FractionalBits to adjust for fixed-point
		if (other.value == 0) {
			throw std::runtime_error("Division by zero, NAN");
		}
		result.value = (this->value << FractionalBits) / other.value;
		return result;
	}

	// Compound assignment operators
	FixedPoint& operator+=(const FixedPoint& other) {
		using PromotedType = typename Promote<StorageType>::type;

		PromotedType this_value = static_cast<PromotedType>(this->value);
		PromotedType other_value = static_cast<PromotedType>(other.value);

		this->value = static_cast<StorageType>(this_value += other_value);
		return *this;
	}

	FixedPoint& operator-=(const FixedPoint& other) {
		using PromotedType = typename Promote<StorageType>::type;

		PromotedType this_value = static_cast<PromotedType>(this->value);
		PromotedType other_value = static_cast<PromotedType>(other.value);

		this->value = static_cast<StorageType>(this_value -= other_value);
		return *this;
	}

	FixedPoint& operator*=(const FixedPoint& other) {
		using PromotedType = typename Promote<StorageType>::type;

		PromotedType this_value = static_cast<PromotedType>(this->value);
		PromotedType other_value = static_cast<PromotedType>(other.value);

		this->value = static_cast<StorageType>((this_value * other_value) >> FractionalBits);
		return *this;
	}

	FixedPoint& operator/=(const FixedPoint& other) {
		if (other.value == 0) {
			throw std::runtime_error("Division by zero, NAN");
		}
		this->value = (this->value << FractionalBits) / other.value;
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
