#include "ccunit.h"
#include <iostream>

int main(int argc, char* argv[]) {

	ccunit::run_benchmarks(std::cout);

	// return ccunit::run_tests(std::cout);
	return 0;
}
