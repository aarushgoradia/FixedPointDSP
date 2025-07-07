#pragma once

#include <concepts>

namespace dsp {

// Must be zero-constructible, + and *–able
template<typename SampleType>
concept Arithmetic =
    requires(SampleType a, SampleType b) {
        { SampleType{ 0 } }      -> std::convertible_to<SampleType>;
        { a + b }              -> std::same_as<SampleType>;
        { a* b }              -> std::same_as<SampleType>;
};

}