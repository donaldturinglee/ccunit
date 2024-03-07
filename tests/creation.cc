#include "ccunit.h"

#include <iostream>

TEST("Test created") {
}

TEST_EX("Test with throw created", int) {
	throw 1;
}
