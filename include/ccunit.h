#ifndef CCUNIT_H
#define CCUNIT_H

#include <string_view>

#define TEST class Test { \
public: \
	Test(std::string_view name) : name_(name), result_(true) {} \
	void operator()(); \
private: \
	std::string name_; \
	bool result_; \
}; \
Test test("test_created");\

void Test::operator()()


#endif // CCUNIT_H
