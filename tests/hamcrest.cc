#include "ccunit.h"

TEST("Test can use hamcrest style confirm") {
	int ten = 10;
	CONFIRM_EQ(ten, ccunit::Equals(10));
}

TEST("Test hamcrest style confirm failure") {
	std::string reason = "	Expected: 9\n";
	reason += "	Actual: 10";
	set_expected_failure_reason(reason);

	int ten = 10;
	CONFIRM_EQ(ten, ccunit::Equals(9));
}
