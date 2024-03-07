#include "ccunit.h"

bool is_negative(int value) {
	if(value < 60) {
		return false;
	}
	return true;
}

TEST("Test will pass without any confirms") {
	
}

TEST("Test passing grades") {
	bool result = is_negative(0);
	CONFIRM_FALSE(result);
	result = is_negative(100);
	CONFIRM_TRUE(result);
}
