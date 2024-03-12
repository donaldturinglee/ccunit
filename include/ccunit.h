#ifndef CCUNIT_H
#define CCUNIT_H

#include <map>
#include <ostream>
#include <string_view>
#include <vector>

namespace ccunit {

class ConfirmException {
public:
	ConfirmException(int line) : line_(line) {}
	virtual ~ConfirmException() = default;
	std::string_view get_reason() const {
		return reason_;
	}
	int get_line() const {
		return line_;
	}
protected:
	std::string reason_;
	int line_;
};

class BoolConfirmException : public ConfirmException {
public:
	BoolConfirmException(bool expected, int line) : ConfirmException(line) {
		reason_ += "	Expected: ";
		reason_ += expected ? "true" : "false";
	}
};

class ActualConfirmException : public ConfirmException {
public:
	ActualConfirmException(std::string_view expected, std::string_view actual, int line) : ConfirmException(line), expected_(expected), actual_(actual) {
		format_reason();
	}
private:
	void format_reason() {
		reason_ += "	Expected: " + expected_ + '\n';
		reason_ += "	Actual: " + actual_;
	}
	std::string expected_;
	std::string actual_;
};

class MissingException {
public:
	MissingException(std::string_view ex_type) : ex_type_(ex_type) {}
	std::string_view get_ex_type() const {
		return ex_type_;
	}
private:
	std::string ex_type_;
};

inline void confirm(bool expected, bool actual, int line) {
	if(actual != expected) {
		throw BoolConfirmException(expected, line);
	}
}

inline void confirm(std::string_view expected, std::string_view actual, int line) {
	if(actual != expected) {
		throw ActualConfirmException(expected, actual, line);
	}
}

inline void confirm(std::string const& expected, std::string const& actual, int line) {
	confirm(std::string_view(expected), std::string_view(actual), line);
}

inline void confirm(float expected, float actual, int line) {
	if(actual < (expected - 0.0001f) || actual > (expected + 0.0001f)) {
		throw ActualConfirmException(std::to_string(expected), std::to_string(actual), line);
	}
}

inline void confirm(double expected, double actual, int line) {
	if(actual < (expected - 0.000001) || actual > (expected + 0.000001)) {
		throw ActualConfirmException(std::to_string(expected), std::to_string(actual), line);
	}
}

inline void confirm(long double expected, long double actual, int line) {
	if(actual < (expected - 0.000001) || actual > (expected + 0.000001)) {
		throw ActualConfirmException(std::to_string(expected), std::to_string(actual), line);
	}
}

template<typename T>
void confirm(T const& expected, T const& actual, int line) {
	if(actual != expected) {
		throw ActualConfirmException(std::to_string(expected), std::to_string(actual), line);
	}
}

class Test;
class TestSuite;

inline std::map<std::string, std::vector<Test*>>& get_tests() {
	static std::map<std::string, std::vector<Test*>> tests;
	return tests;
}

inline std::map<std::string, std::vector<TestSuite*>>& get_test_suites() {
	static std::map<std::string, std::vector<TestSuite*>> suites;
	return suites;
}

inline void add_test(std::string_view suite_name, Test* test) {
	std::string name(suite_name);
	if(!get_tests().contains(name)) {
		get_tests().try_emplace(name, std::vector<Test*>());
	}
	get_tests()[name].push_back(test);
}

inline void add_test_suite(std::string_view suite_name, TestSuite* suite) {
	std::string name(suite_name);
	if(!get_test_suites().contains(name)) {
		get_test_suites().try_emplace(name, std::vector<TestSuite*>());
	}
	get_test_suites()[name].push_back(suite);
}

class TestBase {
public:
	TestBase(std::string_view name, std::string_view suite_name) : name_(name), suite_name_(suite_name), passed_(true), confirm_location_(-1) {}
	virtual ~TestBase() = default;
	std::string_view get_name() const {
		return name_;
	}
	std::string_view get_suite_name() const {
		return suite_name_;
	}
	bool get_passed() const {
		return passed_;
	}
	std::string_view get_reason() const {
		return reason_;
	}
	int get_confirm_location() const {
		return confirm_location_;
	}
	void set_failed(std::string_view reason, int confirm_location = -1) {
		passed_ = false;
		reason_ = reason;
		confirm_location_ = confirm_location;
	}
private:
	std::string name_;
	std::string suite_name_;
	bool passed_;
	std::string reason_;
	int confirm_location_;
};

class Test : public TestBase {
public:
	Test(std::string_view name, std::string_view suite_name) : TestBase(name, suite_name) {
		add_test(suite_name, this);
	}
	virtual void run_ex() {
		run();
	}
	virtual void run() = 0;
	std::string_view get_expected_reason() const {
		return expected_reason_;
	}
	void set_expected_failure_reason(std::string_view reason) {
		expected_reason_ = reason;
	}
private:
	std::string expected_reason_;
};

template<typename ExceptionType>
class TestEx : public Test {
public:
	TestEx(std::string_view name, std::string_view suite_name, std::string_view exception_name) : Test(name, suite_name), exception_name_(exception_name) {}
	void run_ex() override {
		try {
			run();
		} catch(ExceptionType const& ex) {
			return;
		}
		throw MissingException(exception_name_);
	}
private:
	std::string exception_name_;
};

class TestSuite : public TestBase {
public:
	TestSuite(std::string_view name, std::string_view suite_name) : TestBase(name, suite_name) {
		add_test_suite(suite_name, this);
	}
	virtual void suite_setup() = 0;
	virtual void suite_teardown() = 0;
};

inline void run_test(std::ostream& os, Test* test, int& num_passed, int& num_failed, int& num_missed_failed) {
	os << "------- Test: " << test->get_name() << '\n';
	try {
		test->run_ex();
	} catch(ConfirmException const& ex) {
		test->set_failed(ex.get_reason(), ex.get_line());
	} catch(MissingException const& ex) {
		std::string message = "Expected exception type ";
		message += ex.get_ex_type();
		message += " was not thrown.";
		test->set_failed(message);
	} catch(...) {
		test->set_failed("Unexpected exception thrown.");
	}

	if(test->get_passed()) {
		if(!test->get_expected_reason().empty()) {
			++num_missed_failed;
			os << "Missed expected failure\n";
			os << "Test passed but was expected to fail.\n";
		} else {
			++num_passed;
			os << "Passed\n";
		}
	} else if(!test->get_expected_reason().empty() && test->get_expected_reason() == test->get_reason()) {
		++num_passed;
		os << "Expected failure\n";
		os << test->get_reason() << '\n';
	} else {
		++num_failed;
		if(test->get_confirm_location() != -1) {
			os << "Failed confirm on line " << test->get_confirm_location() << '\n';
		} else {
			os << "Failed\n";
		}
		os << test->get_reason() << '\n';
	}
}

inline bool run_suite(std::ostream& os, bool setup, std::string const& name, int& num_passed, int& num_failed) {
	for(auto& suite: get_test_suites()[name]) {
		if(setup) {
			os << "------- Setup: ";
		} else {
			os << "------- Teardown: ";
		}
		os << suite->get_name() << '\n';

		try {
			if(setup) {
				suite->suite_setup();
			} else {
				suite->suite_teardown();
			}
		} catch(ConfirmException const& ex) {
			suite->set_failed(ex.get_reason(), ex.get_line());
		} catch(...) {
			suite->set_failed("Unexpected exception thrown.");
		}

		if(suite->get_passed()) {
			++num_passed;
			os << "Passed\n";
		} else {
			++num_failed;
			if(suite->get_confirm_location() != -1) {
				os << "Failed confirm on line " << suite->get_confirm_location() << '\n';
			} else {
				os << "Failed\n";
			}
			os << suite->get_reason() << '\n';
			return false;
		}
	}
	return true;
}

inline int run_tests(std::ostream& os) {
	os << "Running " << get_tests().size() << " test suites\n";
	int num_passed{0};
	int num_missed_failed{0};
	int num_failed{0};

	for(auto const& [key, value] : get_tests()) {
		std::string suite_display_name = "Suite: ";
		if(key.empty()) {
			suite_display_name += "Single Tests";
		} else {
			suite_display_name += key;
		}
		os << "--------------- " << suite_display_name << '\n';
		if(!key.empty()) {
			if(!get_test_suites().contains(key)) {
				os << "Test suite is not found. Exiting test application.\n";
				return ++num_failed;
			}
			if(!run_suite(os, true, key, num_passed, num_failed)) {
				os << "Test suite setup failed. Skipping tests in suite.\n";
				continue;
			}
		}

		for(auto* test : value) {
			run_test(os, test, num_passed, num_failed, num_missed_failed);
		}

		if(!key.empty()) {
			if(!run_suite(os, false, key, num_passed, num_failed)) {
				os << "Test suite teardown failed.\n";
			}
		}
	}
	os << "-----------------------------------\n";
	os << "Tests passed: " << num_passed << '\n';
	os << "Tests failed: " << num_failed << '\n';

	if(num_missed_failed != 0) {
		os << "Tests failures missed: " << num_missed_failed;
	}
	os << '\n';

	return num_failed;
}

template<typename T>
class SetupAndTeardown : public T {
public:
	SetupAndTeardown() {
		T::setup();
	}
	~SetupAndTeardown() {
		T::teardown();
	}
};

template<typename T>
class TestSuiteSetupAndTeardown : public T, public TestSuite {
public:
	TestSuiteSetupAndTeardown(std::string_view name, std::string_view suite) : TestSuite(name, suite) {}
	void suite_setup() override {
		T::setup();
	}
	void suite_teardown() override {
		T::teardown();
	}
};

} // namespace ccunit

#define CCUNIT_CLASS_FINAL(line) Test ## line
#define CCUNIT_CLASS_RELAY(line) CCUNIT_CLASS_FINAL(line)
#define CCUNIT_CLASS CCUNIT_CLASS_RELAY(__LINE__)

#define CCUNIT_INSTANCE_FINAL(line) test ## line
#define CCUNIT_INSTANCE_RELAY(line) CCUNIT_INSTANCE_FINAL(line)
#define CCUNIT_INSTANCE CCUNIT_INSTANCE_RELAY(__LINE__)

#define TEST_SUITE(test_name, suite_name) \
namespace { \
class CCUNIT_CLASS : public ccunit::Test { \
public: \
	CCUNIT_CLASS(std::string_view name, std::string_view suite) : Test(name, suite) {} \
	void run() override; \
}; \
} /* unnamed namespace */ \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name, suite_name); \
void CCUNIT_CLASS::run()

#define TEST_SUITE_EX(test_name, suite_name, exception_type) \
namespace { \
class CCUNIT_CLASS : public ccunit::TestEx<exception_type> { \
public: \
	CCUNIT_CLASS(std::string_view name, std::string_view suite, std::string_view exception_name) : TestEx(name, suite, exception_name) {} \
	void run() override; \
}; \
} /* unnamed namespace */ \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name, suite_name, #exception_type); \
void CCUNIT_CLASS::run()

#define TEST(test_name) \
namespace { \
class CCUNIT_CLASS : public ccunit::Test { \
public: \
	CCUNIT_CLASS(std::string_view name) : Test(name, "") {} \
	void run() override; \
}; \
} /* unnamed namespace */ \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name); \
void CCUNIT_CLASS::run()

#define TEST_EX(test_name, exception_type) \
namespace { \
class CCUNIT_CLASS : public ccunit::TestEx<exception_type> { \
public: \
	CCUNIT_CLASS(std::string_view name, std::string_view exception_name) : TestEx(name, "", exception_name) {} \
	void run() override; \
}; \
} /* unnamed namespace */ \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name, #exception_type); \
void CCUNIT_CLASS::run()

#define CONFIRM_FALSE(actual) \
	ccunit::confirm(false, actual, __LINE__)
#define CONFIRM_TRUE(actual) \
	ccunit::confirm(true, actual, __LINE__)
#define CONFIRM(expected, actual) \
	ccunit::confirm(expected, actual, __LINE__)

#endif // CCUNIT_H
