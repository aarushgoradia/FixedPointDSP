#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>
#include <algorithm>
#include "promote.hpp"

// Default wrap-around policy, this is standard C++ behavior for arithmetic operations.
template<typename StorageType, typename Promote, int FractionalBits>
struct WrapAroundPolicy {
	template <typename T>
	static T add(T a, T b) {
		return a + b;
	}

	template <typename T>
	static T sub(T a, T b) {
		return a - b;
	}

	template <typename T>
	static T mul(T a, T b) {
		return (a * b) >> fractional_bits;
	}

	template <typename T>
	static T div(T a, T b) {
		if (b == 0) {
			throw std::runtime_error("Division by zero");
		}
		return (a << fractional_bits) / b;
	}
};

// Saturation policy: This keeps the maximum or minimum value of the type when an overflow or underflow occurs.
template<typename StorageType, typename Promote, int FractionalBits>
struct SaturationPolicy {
	using Wide = typename Promote<StorageType>::type;
	
	static StorageType add(StorageType a, StorageType b) {

		Wide result = static_cast<Wide>(a) + static_cast<Wide>(b);

		if (result > std::numeric_limits<StorageType>::max()) {
			return std::numeric_limits<StorageType>::max();

		} else if (result < std::numeric_limits<StorageType>::min()) {
			return std::numeric_limits<StorageType>::min();
		}

		return static_cast<StorageType>(result);
	}

	static StorageType sub(StorageType a, StorageType b) {

		Wide result = static_cast<Wide>(a) - static_cast<Wide>(b);

		if (result > std::numeric_limits<StorageType>::max()) {
			return std::numeric_limits<StorageType>::max();

		} else if (result < std::numeric_limits<StorageType>::min()) {
			return std::numeric_limits<StorageType>::min();
		}

		return static_cast<StorageType>(result);
	}

	static StorageType mul(StorageType a, StorageType b) {

		Wide result = (static_cast<Wide>(a) * static_cast<Wide>(b)) >> FractionalBits;

		if (result > std::numeric_limits<StorageType>::max()) {
			return std::numeric_limits<StorageType>::max();
		} else if (result < std::numeric_limits<StorageType>::min()) {
			return std::numeric_limits<StorageType>::min();
		}

		return static_cast<StorageType>(result);
	}

	static StorageType div(StorageType a, StorageType b) {
		if (b == 0) {
			throw std::runtime_error("Division by zero");
		}

		Wide result = (static_cast<Wide>(a) << FractionalBits) / static_cast<Wide>(b);

		if (result > std::numeric_limits<StorageType>::max()) {
			return std::numeric_limits<StorageType>::max();
		} else if (result < std::numeric_limits<StorageType>::min()) {
			return std::numeric_limits<StorageType>::min();
		}

		return static_cast<StorageType>(result);
	}
};