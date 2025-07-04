#include "fixed_point/fixed_point.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <limits>

// Type aliases for both policies
using MyFixedSaturate = FixedPoint<16, 8, SaturationPolicy>;
using MyFixedWrap = FixedPoint<16, 8, WrapAroundPolicy>;

// Helper to get min/max for the storage type
constexpr auto kMin = std::numeric_limits<typename MyFixedSaturate::StorageType>::min();
constexpr auto kMax = std::numeric_limits<typename MyFixedSaturate::StorageType>::max();

TEST(FixedPointTest, ConstructorFromInt) {
    MyFixedSaturate fp(2);
    EXPECT_EQ(fp.raw(), 512);
    EXPECT_EQ(fp.to_int(), 2);

    MyFixedSaturate fp_zero(0);
    EXPECT_EQ(fp_zero.raw(), 0);
    EXPECT_EQ(fp_zero.to_int(), 0);

    MyFixedSaturate fp_negative(-2);
    EXPECT_EQ(fp_negative.raw(), -512);
    EXPECT_EQ(fp_negative.to_int(), -2);
}

TEST(FixedPointTest, ConstructorFromFloat) {
    MyFixedSaturate fp(2.5f);
    EXPECT_EQ(fp.raw(), 640);
    EXPECT_FLOAT_EQ(fp.to_float(), 2.5f);

    MyFixedSaturate fp_zero(0.0f);
    EXPECT_EQ(fp_zero.raw(), 0);
    EXPECT_FLOAT_EQ(fp.to_float(), 2.5f);
    EXPECT_FLOAT_EQ(fp_zero.to_float(), 0.0f);

    MyFixedSaturate fp_negative(-2.5f);
    EXPECT_EQ(fp_negative.raw(), -640);
    EXPECT_FLOAT_EQ(fp_negative.to_float(), -2.5f);
}

TEST(FixedPointTest, ConstructorFromDouble) {
    MyFixedSaturate fp(2.5);
    EXPECT_EQ(fp.raw(), 640);
    EXPECT_DOUBLE_EQ(fp.to_double(), 2.5);

    MyFixedSaturate fp_zero(0.0);
    EXPECT_EQ(fp_zero.raw(), 0);
    EXPECT_DOUBLE_EQ(fp_zero.to_double(), 0.0);

    MyFixedSaturate fp_negative(-2.5);
    EXPECT_EQ(fp_negative.raw(), -640);
    EXPECT_DOUBLE_EQ(fp_negative.to_double(), -2.5);
}

TEST(FixedPointTest, ConstructorFromRaw) {
    auto fp = MyFixedSaturate::from_raw(640);
    EXPECT_EQ(fp.raw(), 640);
    EXPECT_FLOAT_EQ(fp.to_float(), 2.5f);
}

TEST(FixedPointTest, RawAccess) {
    MyFixedSaturate fp(2.5f);
    EXPECT_EQ(fp.raw(), 640);
}

TEST(FixedPointTest, Addition) {
    MyFixedSaturate a(2.5f), b(1.5f);
    MyFixedSaturate result = a + b;
    EXPECT_FLOAT_EQ(result.to_float(), 4.0f);
}

TEST(FixedPointTest, Subtraction) {
    MyFixedSaturate a(2.5f), b(1.5f);
    MyFixedSaturate result = a - b;
    EXPECT_FLOAT_EQ(result.to_float(), 1.0f);
}

TEST(FixedPointTest, Multiplication) {
    MyFixedSaturate a(2.0f), b(1.5f);
    MyFixedSaturate result = a * b;
    EXPECT_FLOAT_EQ(result.to_float(), 3.0f);
}

TEST(FixedPointTest, Division) {
    MyFixedSaturate a(2.0f), b(1.0f);
    MyFixedSaturate result = a / b;
    EXPECT_FLOAT_EQ(result.to_float(), 2.0f);

    MyFixedSaturate zero(0.0f);
    EXPECT_THROW(a / zero, std::runtime_error);
}

TEST(FixedPointTest, CompoundOperators) {
    MyFixedSaturate a(2.0f);
    a += MyFixedSaturate(1.0f);
    EXPECT_FLOAT_EQ(a.to_float(), 3.0f);

    a -= MyFixedSaturate(0.5f);
    EXPECT_FLOAT_EQ(a.to_float(), 2.5f);

    a *= MyFixedSaturate(2.0f);
    EXPECT_FLOAT_EQ(a.to_float(), 5.0f);

    a /= MyFixedSaturate(2.0f);
    EXPECT_FLOAT_EQ(a.to_float(), 2.5f);
}

TEST(FixedPointTest, ComparisonOperators) {
    MyFixedSaturate a(2.0f), b(3.0f), c(2.0f);
    EXPECT_TRUE(a == c);
    EXPECT_TRUE(a != b);
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(b >= a);
}

TEST(FixedPointTest, StreamOutput) {
    MyFixedSaturate fp(2.5f);
    std::ostringstream oss;
    oss << fp;
    EXPECT_EQ(oss.str(), "2.5");
}

// ----------------- Edge and Policy Tests -----------------

TEST(FixedPointTest, SaturationPolicy_Overflow) {
    MyFixedSaturate max_fp = MyFixedSaturate::from_raw(kMax);
    MyFixedSaturate one(1.0f);
    auto result = max_fp + one;
    EXPECT_EQ(result.raw(), kMax);

    MyFixedSaturate min_fp = MyFixedSaturate::from_raw(kMin);
    MyFixedSaturate minus_one(-1.0f);
    result = min_fp + minus_one;
    EXPECT_EQ(result.raw(), kMin);
}

TEST(FixedPointTest, SaturationPolicy_Underflow) {
    MyFixedSaturate min_fp = MyFixedSaturate::from_raw(kMin);
    MyFixedSaturate one(1.0f);
    auto result = min_fp - one;
    EXPECT_EQ(result.raw(), kMin);
}

TEST(FixedPointTest, SaturationPolicy_MulOverflow) {
    MyFixedSaturate a = MyFixedSaturate::from_raw(kMax);
    MyFixedSaturate b(2.0f);
    auto result = a * b;
    EXPECT_EQ(result.raw(), kMax);
}

TEST(FixedPointTest, SaturationPolicy_DivUnderflow) {
    MyFixedSaturate a = MyFixedSaturate::from_raw(kMin);
    MyFixedSaturate b(2.0f);
    auto result = a / b;
    // Should saturate to min if result is below min
    EXPECT_EQ(result.raw(), kMin);
}

TEST(FixedPointTest, WrapAroundPolicy_Overflow) {
    MyFixedWrap max_fp = MyFixedWrap::from_raw(kMax);
    MyFixedWrap one(1.0f);
    auto result = max_fp + one;
    // Wraps around to min
    EXPECT_EQ(result.raw(), kMin);
}

TEST(FixedPointTest, WrapAroundPolicy_Underflow) {
    MyFixedWrap min_fp = MyFixedWrap::from_raw(kMin);
    MyFixedWrap minus_one(-1.0f);
    auto result = min_fp + minus_one;
    // Wraps around to max
    EXPECT_EQ(result.raw(), kMax);
}

TEST(FixedPointTest, WrapAroundPolicy_MulOverflow) {
    MyFixedWrap a = MyFixedWrap::from_raw(kMax);
    MyFixedWrap b(2.0f);
    auto result = a * b;
    // Wraps around, not saturate
    EXPECT_NE(result.raw(), kMax);
}

TEST(FixedPointTest, WrapAroundPolicy_DivByZero) {
    MyFixedWrap a(1.0f), zero(0.0f);
    EXPECT_THROW(a / zero, std::runtime_error);
}

TEST(FixedPointTest, EdgeCase_Zero) {
    MyFixedSaturate zero(0.0f);
    MyFixedSaturate any(123.456f);
    EXPECT_EQ((zero + zero).raw(), 0);
    EXPECT_EQ((zero - zero).raw(), 0);
    EXPECT_EQ((zero * any).raw(), 0);
    EXPECT_EQ((any * zero).raw(), 0);
}

TEST(FixedPointTest, EdgeCase_MaxMin) {
    MyFixedSaturate max_fp = MyFixedSaturate::from_raw(kMax);
    MyFixedSaturate min_fp = MyFixedSaturate::from_raw(kMin);
    EXPECT_GT(max_fp, min_fp);
    EXPECT_LT(min_fp, max_fp);
    EXPECT_FALSE(max_fp == min_fp);
}
