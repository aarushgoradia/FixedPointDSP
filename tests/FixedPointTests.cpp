#include "fixed_point/fixed_point.hpp"  
#include <gtest/gtest.h>  

TEST(FixedPointTest, ConstructorFromInt) {
	FixedPoint<16, 8> fp(2);
	EXPECT_EQ(fp.raw(), 512);
	EXPECT_EQ(fp.to_int(), 2);

	FixedPoint<16, 8> fp_zero(0);
	EXPECT_EQ(fp_zero.raw(), 0);
	EXPECT_EQ(fp_zero.to_int(), 0);

	FixedPoint<16, 8> fp_negative(-2);
	EXPECT_EQ(fp_negative.raw(), -512);
	EXPECT_EQ(fp_negative.to_int(), -2);
}

TEST(FixedPointTest, ConstructorFromFloat) {
	FixedPoint<16, 8> fp(2.5f);
	EXPECT_EQ(fp.raw(), 640);
	EXPECT_FLOAT_EQ(fp.to_float(), 2.5f);

	FixedPoint<16, 8> fp_zero(0.0f);
	EXPECT_EQ(fp_zero.raw(), 0);
	EXPECT_FLOAT_EQ(fp_zero.to_float(), 0.0f);

	FixedPoint<16, 8> fp_negative(-2.5f);
	EXPECT_EQ(fp_negative.raw(), -640);
	EXPECT_FLOAT_EQ(fp_negative.to_float(), -2.5f);
}

TEST(FixedPointTest, ConstructorFromDouble) {
	FixedPoint<16, 8> fp(2.5);
	EXPECT_EQ(fp.raw(), 640);
	EXPECT_DOUBLE_EQ(fp.to_double(), 2.5);

	FixedPoint<16, 8> fp_zero(0.0);
	EXPECT_EQ(fp_zero.raw(), 0);
	EXPECT_DOUBLE_EQ(fp_zero.to_double(), 0.0);

	FixedPoint<16, 8> fp_negative(-2.5);
	EXPECT_EQ(fp_negative.raw(), -640);
	EXPECT_DOUBLE_EQ(fp_negative.to_double(), -2.5);
}

TEST(FixedPointTest, ConstructorFromRaw) {
	auto fp = FixedPoint<16, 8>::from_raw(640);
	EXPECT_EQ(fp.raw(), 640);
	EXPECT_FLOAT_EQ(fp.to_float(), 2.5f);
}

TEST(FixedPointTest, RawAccess) {
	FixedPoint<16, 8> fp(2.5f);
	EXPECT_EQ(fp.raw(), 640);
}

TEST(FixedPointTest, Addition) {
	FixedPoint<16, 8> fp1(2.5f);
	FixedPoint<16, 8> fp2(1.5f);
	FixedPoint<16, 8> result = fp1 + fp2;
	EXPECT_EQ(result.raw(), 640 + 384); 
	EXPECT_FLOAT_EQ(result.to_float(), 4.0f);

	FixedPoint<16, 8> fp_zero(0.0f);
	result = fp1 + fp_zero;
	EXPECT_EQ(result.raw(), 640);
	EXPECT_FLOAT_EQ(result.to_float(), 2.5f);

	FixedPoint<16, 8> fp_negative(-1.5f);
	result = fp1 + fp_negative;
	EXPECT_EQ(result.raw(), 640 - 384);

	FixedPoint<16, 8> overflow1(100.0f);
	FixedPoint<16, 8> overflow2(40.0f);
	result = overflow1 + overflow2;
	EXPECT_EQ(result.raw(), -29696);
	EXPECT_NEAR(result.to_float(), -29696.0f / 256.0f, 0.01);
}

TEST(FixedPointTest, Subtraction) {
	FixedPoint<16, 8> fp1(2.5f);
	FixedPoint<16, 8> fp2(1.5f);
	FixedPoint<16, 8> result = fp1 - fp2;
	EXPECT_EQ(result.raw(), 640 - 384); 
	EXPECT_FLOAT_EQ(result.to_float(), 1.0f);

	FixedPoint<16, 8> fp_zero(0.0f);
	result = fp1 - fp_zero;
	EXPECT_EQ(result.raw(), 640);
	EXPECT_FLOAT_EQ(result.to_float(), 2.5f);

	FixedPoint<16, 8> fp_negative(-1.5f);
	result = fp1 - fp_negative;
	EXPECT_EQ(result.raw(), 640 + 384);

	FixedPoint<16, 8> overflow1(-100.0f);
	FixedPoint<16, 8> overflow2(40.0f);
	result = overflow1 - overflow2;
	EXPECT_EQ(result.raw(), 29696);
	EXPECT_NEAR(result.to_float(), 29696.0f / 256.0f, 0.01);
}

TEST(FixedPointTest, Multiplication) {
	FixedPoint<16, 8> fp1(2.0f);
	FixedPoint<16, 8> fp2(1.5f);
	FixedPoint<16, 8> result = fp1 * fp2;
	EXPECT_EQ(result.raw(), 768); // (2.0 * 1.5) * 256
	EXPECT_FLOAT_EQ(result.to_float(), 3.0f);

	FixedPoint<16, 8> fp_zero(0.0f);
	result = fp1 * fp_zero;
	EXPECT_EQ(result.raw(), 0);
	EXPECT_FLOAT_EQ(result.to_float(), 0.0f);

	FixedPoint<16, 8> fp_negative(-1.5f);
	result = fp1 * fp_negative;
	EXPECT_EQ(result.raw(), -384); // (2.0 * -1.5) * 256

	FixedPoint<16, 8> overflow1(16.0f);
	FixedPoint<16, 8> overflow2(8.0f);
	result = overflow1 * overflow2;
	EXPECT_EQ(result.raw(), -32768);
	EXPECT_NEAR(result.to_float(), -32768.0f / 256.0f, 0.01);  // ≈ -128.0f
}

TEST(FixedPointTest, Division) {
	FixedPoint<16, 8> fp1(2.0f);
	FixedPoint<16, 8> fp2(1.0f);
	FixedPoint<16, 8> result = fp1 / fp2;
	EXPECT_EQ(result.raw(), 512); // (2.0 / 1.0) * 256
	EXPECT_FLOAT_EQ(result.to_float(), 2.0f);

	FixedPoint<16, 8> fp_zero(0.0f);
	result = fp1 / fp_zero; // Should throw an exception
	EXPECT_THROW(fp1 / fp_zero, std::runtime_error);

	FixedPoint<16, 8> fp_negative(-1.0f);
	result = fp1 / fp_negative;
	EXPECT_EQ(result.raw(), -512); // (2.0 / -1.0) * 256
}

TEST(FixedPointTest, CompoundAddition) {
	FixedPoint<16, 8> fp(2.0f);
	fp += FixedPoint<16, 8>(1.0f);
	EXPECT_FLOAT_EQ(fp.to_float(), 3.0f);
}

TEST(FixedPointTest, CompoundSubtraction) {
	FixedPoint<16, 8> fp(2.0f);
	fp -= FixedPoint<16, 8>(1.0f);
	EXPECT_FLOAT_EQ(fp.to_float(), 1.0f);
}

TEST(FixedPointTest, CompoundMultiplication) {
	FixedPoint<16, 8> fp(2.0f);
	fp *= FixedPoint<16, 8>(1.5f);
	EXPECT_FLOAT_EQ(fp.to_float(), 3.0f);
}

TEST(FixedPointTest, CompoundDivision) {
	FixedPoint<16, 8> fp(2.0f);
	fp /= FixedPoint<16, 8>(2.0f);
	EXPECT_FLOAT_EQ(fp.to_float(), 1.0f);
}

TEST(FixedPointTest, ComparisonOperators) {
	FixedPoint<16, 8> a(2.0f);
	FixedPoint<16, 8> b(3.0f);
	FixedPoint<16, 8> c(2.0f);

	EXPECT_TRUE(a == c);
	EXPECT_TRUE(a != b);
	EXPECT_TRUE(a < b);
	EXPECT_TRUE(a <= b);
	EXPECT_TRUE(b > a);
	EXPECT_TRUE(b >= a);
}

TEST(FixedPointTest, StreamOutput) {
	FixedPoint<16, 8> fp(2.5f);
	std::ostringstream oss;
	oss << fp;
	EXPECT_EQ(oss.str(), "2.5");
}
