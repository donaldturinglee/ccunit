#include "ccunit.h"

bool is_negative(int value) {
	if(value < 60) {
		return false;
	}
	return true;
}

int multiply_by_2(int value) {
	return value * 2;
}

long multiply_by_2(long value) {
	return value * 2L;
}

long long multiply_by_2(long long value) {
	return value * 2LL;
}

TEST("Test will pass without any confirms") {
	
}

TEST("Test passing grades") {
	bool result = is_negative(0);
	CONFIRM_FALSE(result);
	result = is_negative(100);
	CONFIRM_TRUE(result);
}

TEST("Test int confirms") {
	int result = multiply_by_2(0);
	CONFIRM(0, result);
	result = multiply_by_2(1);
	CONFIRM(2, result);
	result = multiply_by_2(-1);
	CONFIRM(-2, result);
}

TEST("Test bool confirm failure") {
	std::string reason = "    Expected: true";
	set_expected_failure_reason(reason);
	bool result = is_negative(0);
	CONFIRM_TRUE(result);
}

TEST("Test int confirm failure") {
	std::string reason = "    Expected: 0\n";
	reason += "    Actual  : 2";
	set_expected_failure_reason(reason);
	int result = multiply_by_2(1);
	CONFIRM(0, result);
}

TEST("Test long confirms") {
	long result = multiply_by_2(0L);
	CONFIRM(0L, result);
	result = multiply_by_2(1L);
	CONFIRM(2L, result);
	result = multiply_by_2(-1L);
	CONFIRM(-2L, result);
}

TEST("Test long confirm failure") {
	std::string reason = "    Expected: 0\n";
	reason += "    Actual  : 2";
	set_expected_failure_reason(reason);
	long result = multiply_by_2(1L);
	CONFIRM(0L, result);
}

TEST("Test long long confirms") {
	long long result = multiply_by_2(0LL);
	CONFIRM(0LL, result);
	result = multiply_by_2(10'000'000'000LL);
	CONFIRM(20'000'000'000LL, result);
	result = multiply_by_2(-10'000'000'000LL);
	CONFIRM(-20'000'000'000LL, result);
}

TEST("Test long long confirm failure") {
	std::string reason = "    Expected: 10000000000\n";
	reason += "    Actual  : 20000000000";
	set_expected_failure_reason(reason);
	long long result = multiply_by_2(10'000'000'000LL);
	CONFIRM(10'000'000'000LL, result);
}

TEST("Test string confirms") {
	std::string result = "abc";
	std::string expected = "abc";
	CONFIRM(expected, result);
}

TEST("Test string confirms") {
	std::string reason = "    Expected: def\n";
	reason += "    Actual  : abc";
	set_expected_failure_reason(reason);
	std::string result = "abc";
	std::string expected = "def";
	CONFIRM(expected, result);
}

TEST("Test bool pointer dereference confirms") {
	bool result1 = true;
	bool result2 = false;

	bool* result1_ptr = &result1;
	CONFIRM_TRUE(*result1_ptr);
	bool* result2_ptr = &result2;
	CONFIRM_FALSE(*result2_ptr);
}

TEST("Test string and string literal confirms") {
	std::string result = "abc";
	CONFIRM("abc", result);
}

TEST("Test float confirms") {
	float f1 = 0.1f;
	float f2 = 0.2f;
	float sum = f1 + f2;
	float expected = 0.3f;
	CONFIRM(expected, sum);
}

TEST("Test double confirms") {
	double d1 = 0.1;
	double d2 = 0.2;
	double sum = d1 + d2;
	double expected = 0.3;
	CONFIRM(expected, sum);
}

TEST("Test double confirms") {
	long double ld1 = 0.1;
	long double ld2 = 0.2;
	long double sum = ld1 + ld2;
	long double expected = 0.3;
	CONFIRM(expected, sum);
}
