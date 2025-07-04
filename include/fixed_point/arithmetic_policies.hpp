#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>
#include <algorithm>
#include <stdexcept>
#include "promote.hpp"

// Default wrap-around policy, this is standard C++ behavior for arithmetic operations.
template<typename StorageType, template<typename> class Promote, int FractionalBits>
struct WrapAroundPolicy {
    using Wide = typename Promote<StorageType>::type;

    static StorageType add(StorageType a, StorageType b) {
        Wide r = static_cast<Wide>(a) + static_cast<Wide>(b);
        constexpr auto min = std::numeric_limits<StorageType>::min();
        constexpr auto max = std::numeric_limits<StorageType>::max();
        if (r > max) return min;      // wrap to floor
        if (r < min) return max;      // wrap to ceiling
        return static_cast<StorageType>(r);
    }

    static StorageType sub(StorageType a, StorageType b) {
        Wide r = static_cast<Wide>(a) - static_cast<Wide>(b);
        constexpr auto min = std::numeric_limits<StorageType>::min();
        constexpr auto max = std::numeric_limits<StorageType>::max();
        if (r > max) return min;
        if (r < min) return max;
        return static_cast<StorageType>(r);
    }

    static StorageType mul(StorageType a, StorageType b) {
        // full-precision multiply, then shift off fractional bits
        Wide r = (static_cast<Wide>(a) * static_cast<Wide>(b)) >> FractionalBits;
        constexpr auto min = std::numeric_limits<StorageType>::min();
        constexpr auto max = std::numeric_limits<StorageType>::max();
        if (r > max) return min;
        if (r < min) return max;
        return static_cast<StorageType>(r);
    }

    static StorageType div(StorageType a, StorageType b) {
        if (b == 0) throw std::runtime_error("Division by zero");
        Wide r = (static_cast<Wide>(a) << FractionalBits) / static_cast<Wide>(b);
        constexpr auto min = std::numeric_limits<StorageType>::min();
        constexpr auto max = std::numeric_limits<StorageType>::max();
        if (r > max) return min;
        if (r < min) return max;
        return static_cast<StorageType>(r);
    }
};


// Saturation policy: This keeps the maximum or minimum value of the type when an overflow or underflow occurs.
template<typename StorageType, template<typename> class Promote, int FractionalBits>
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
        if (b == 0) throw std::runtime_error("Division by zero");

        constexpr auto min = std::numeric_limits<StorageType>::min();
        constexpr auto max = std::numeric_limits<StorageType>::max();

        if (a == min) {
            return min;
        }

        Wide result = (static_cast<Wide>(a) << FractionalBits)
                      / static_cast<Wide>(b);

        if      (result > max) return max;
        else if (result < min) return min;
        else                   return static_cast<StorageType>(result);
	}
};