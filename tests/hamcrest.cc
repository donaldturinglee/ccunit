#include "ccunit.h"

using namespace ccunit;

template<typename T>
T calculate_fraction(T input) {
    T denominator{10};
    return input / denominator;
}

template<typename T>
T accumulate_error (T input) {
    // First add many small amounts.
    T partial_amount{0.1};
    for(int i = 0; i < 10; ++i) {
        input += partial_amount;
    }
    // Then subtract to get back to the original.
    T whole_amount {1};
    input -= whole_amount;
    return input;
}

template<typename T>
int perform_comparisons(int total_count) {
    int pass_count{0};
    for(int i = 0; i < total_count; ++i) {
        T expected = static_cast<T>(i);
        expected = calculate_fraction(expected);
        T actual = accumulate_error(expected);
        if (compare_eq(actual, expected))
        {
            ++pass_count;
        }
    }
    return pass_count;
}

TEST("Test can use hamcrest style confirm") {
	int ten = 10;
	CONFIRM_EQ(ten, Equals(10));
}

TEST("Test hamcrest style confirm failure") {
	std::string reason = "	Expected: 9\n";
	reason += "	Actual: 10";
	set_expected_failure_reason(reason);

	int ten = 10;
	CONFIRM_EQ(ten, Equals(9));
}

TEST("Test other hamcrest style integer confirms") {
	char c1 = 'A';
	char c2 = 'A';
	CONFIRM_EQ(c1, Equals(c2));
	CONFIRM_EQ(c1, Equals('A'));

	short s1 = 10;
	short s2 = 10;
	CONFIRM_EQ(s1, Equals(s2));
	CONFIRM_EQ(s1, Equals(10));

	unsigned int ui1 = 3'000'000'000;
	unsigned int ui2 = 3'000'000'000;
	CONFIRM_EQ(ui1, Equals(ui2));
	CONFIRM_EQ(ui1, Equals(3'000'000'000));

	long long ll1 = 5'000'000'000'000LL;
	long long ll2 = 5'000'000'000'000LL;
	CONFIRM_EQ(ll1, Equals(ll2));
	CONFIRM_EQ(ll1, Equals(5'000'000'000'000LL));
}

TEST("Test hamcrest style bool confirms")
{
    bool b1 = true;
    bool b2 = true;
    CONFIRM_EQ(b1, Equals(b2));

    // This works but probably won't be used much.
    CONFIRM_EQ(b1, Equals(true));

    // When checking a bool variable for a known value,
    // the classic style is probably better.
    CONFIRM_TRUE(b1);
}

TEST("Test hamcrest style string confirms") {
	std::string s1 = "abc";
	std::string s2 = "abc";
    CONFIRM_EQ(s1, Equals(s2));       // string vs. string
    CONFIRM_EQ(s1, Equals("abc"));    // string vs. literal
    CONFIRM_EQ("abc", Equals(s1));    // literal vs. string

    // Probably not needed, but this works too.
    CONFIRM_EQ("abc", Equals("abc")); // literal vs. literal

    std::string s3 = "def";
    CONFIRM_EQ(s1, NotEquals(s3));       // string vs. string
    CONFIRM_EQ(s1, NotEquals("def"));    // string vs. literal
    CONFIRM_EQ("def", NotEquals(s1));    // literal vs. string
}

TEST("Test hamcrest style string pointer confirms")
{
    char const* sp1 = "abc";
    std::string s1 = "abc";
    char const* sp2 = s1.c_str();    // avoid sp1 and sp2 being same
    CONFIRM_EQ(sp1, Equals(sp2));   // pointer vs. pointer
    CONFIRM_EQ(sp2, Equals("abc")); // pointer vs. literal
    CONFIRM_EQ("abc", Equals(sp2)); // literal vs. pointer
    CONFIRM_EQ(sp1, Equals(s1));    // pointer vs. string
    CONFIRM_EQ(s1, Equals(sp1));    // string vs. pointer

    char const* sp3 = "def";
    CONFIRM_EQ(sp1, NotEquals(sp3));   // pointer vs. pointer
    CONFIRM_EQ(sp1, NotEquals("def")); // pointer vs. literal
    CONFIRM_EQ("def", NotEquals(sp1)); // literal vs. pointer
    CONFIRM_EQ(sp3, NotEquals(s1));    // pointer vs. string
    CONFIRM_EQ(s1, NotEquals(sp3));    // string vs. pointer
}

TEST("Test many float comparisons")
{
    int total_count {1'000};
    int pass_count = perform_comparisons<float>(total_count);
    CONFIRM_EQ(pass_count, Equals(total_count));
}

TEST("Test many double comparisons")
{
    int total_count{1'000};
    int pass_count = perform_comparisons<double>(total_count);
    CONFIRM_EQ(pass_count, Equals(total_count));
}

TEST("Test many long double comparisons") {
    int total_count{1'000};
    int pass_count = perform_comparisons<long double>(total_count);
    CONFIRM_EQ(pass_count, Equals(total_count));
}

TEST("Test small float values") {
    // Based on float epsilon = 1.1920928955078125e-07
    CONFIRM_EQ(0.000001f, NotEquals(0.000002f));
}

TEST("Test large float values") {
    // Based on float epsilon = 1.1920928955078125e-07
    CONFIRM_EQ(9'999.0f, Equals(9'999.001f));
}

TEST("Test small double values") {
    // Based on double epsilon = 2.2204460492503130808e-16
    CONFIRM_EQ(0.000000000000001, NotEquals(0.000000000000002));
}

TEST("Test large double values") {
    // Based on double epsilon = 2.2204460492503130808e-16
    CONFIRM_EQ(1'500'000'000'000.0, Equals(1'500'000'000'000.0003));
}

TEST("Test small long double values") {
    // Based on double epsilon = 2.2204460492503130808e-16
    CONFIRM_EQ(0.000000000000001L, NotEquals(0.000000000000002L));
}

TEST("Test large long double values") {
    // Based on double epsilon = 2.2204460492503130808e-16
    CONFIRM_EQ(1'500'000'000'000.0L, Equals(1'500'000'000'000.0003L));
}

TEST("Test even integral value") {
    CONFIRM_EQ(10, IsEven<int>());
}

TEST("Test even integral value confirm failure") {
    std::string reason = "	Expected: is even\n";
    reason += "	Actual: 11";
    set_expected_failure_reason(reason);

    CONFIRM_EQ(11, IsEven<int>());
}
