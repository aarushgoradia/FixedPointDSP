#pragma once

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