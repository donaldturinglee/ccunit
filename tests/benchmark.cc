#include "ccunit.h"

void sum() {
	int sum = 0;
	for(int i = 0; i < 1000; ++i) {
		sum += i;
	}
}

void sum2() {
	int sum = 0;
	for(int i = 0; i < 10000; ++i) {
		sum += i;
	}
}
void sum3() {
	int sum = 0;
	for(int i = 0; i < 100000; ++i) {
		sum += i;
	}
}

BENCHMARK("test", sum);
BENCHMARK("test2", sum2);
BENCHMARK("test3", sum3);
