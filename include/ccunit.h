#ifndef CCUNIT_H
#define CCUNIT_H

#include <ostream>
#include <string_view>
#include <source_location>
#include <vector>

namespace ccunit {
	class TestBase;
	inline std::vector<TestBase*>& get_tests() {
		static std::vector<TestBase*> tests;
		return tests;
	}
	class TestBase {
	public:
		TestBase(std::string_view name) : name_(name), passed_(true), confirm_location_(-1) {
			get_tests().push_back(this);
		}
		virtual ~TestBase() = default;
		virtual void run_ex() {
			run();
		}
		virtual void run() = 0;
		std::string_view get_name() const {
			return name_;
		}
		bool get_passed() const {
			return passed_;
		}
		std::string_view get_reason() const {
			return reason_;
		}
		void set_failed(std::string_view reason, int confirm_location = -1) {
			passed_ = false;
			reason_ = reason;
			confirm_location_ = confirm_location;
		}
		std::string_view get_expected_reason() const {
			return expected_reason_;
		}
		void set_expected_failure_reason(std::string_view reason) {
			expected_reason_ = reason;
		}
		int get_confirm_location() const {
			return confirm_location_;
		}
	private:
		std::string name_;
		bool passed_;
		std::string reason_;
		std::string expected_reason_;
		int confirm_location_;
	};


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
			reason_ += "    Expected: ";
			reason_ += expected ? "true" : "false";
		}
	};

	class ActualConfirmException : public ConfirmException {
	public:
		ActualConfirmException(std::string_view expected, std::string_view actual, int line) 
			: ConfirmException(line), expected_(expected), actual_(actual) {
				format_reason();
			}
	private:
		void format_reason() {
			reason_ += "    Expected: " + expected_ + '\n';
			reason_ += "    Actual  : " + actual_;
		}
		std::string expected_;
		std::string actual_;
		int line_;
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

	template<typename ExceptionType>
	class TestExBase : public TestBase {
	public:
		TestExBase(std::string_view name, std::string_view exception_name) : TestBase(name), exception_name_(exception_name) {}
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
	void confirm(T const& expected, T const& actual, int line) {
		if(actual != expected) {
			throw ActualConfirmException(std::to_string(expected), std::to_string(actual), line);
		}
	}

	inline void confirm(bool expected, bool actual, const std::source_location location = std::source_location::current()) {
		if(actual != expected) {
			throw BoolConfirmException(expected, location.line());
		}
	}

	inline void confirm(std::string_view expected, std::string_view actual, int line) {
		if(actual != expected) {
			throw ActualConfirmException(expected, actual, line);
		}
	}

	inline void confirm(std::string& expected, std::string const& actual, int line) {
		confirm(std::string_view(expected), std::string_view(actual), line);
	}

	inline void confirm(float expected, float actual, int line) {
		if(actual < (expected - 0.0001f) || actual > (expected + 0.0001f)) {
			throw ActualConfirmException(std::to_string(expected), std::to_string(line), line);
		}
	}

	inline void confirm(double expected, double actual, int line) {
		if(actual < (expected - 0.000001) || actual > (expected + 0.000001)) {
			throw ActualConfirmException(std::to_string(expected), std::to_string(line), line);
		}
	}

	inline void confirm(long double expected, long double actual, int line) {
		if(actual < (expected - 0.000001) || actual > (expected + 0.000001)) {
			throw ActualConfirmException(std::to_string(expected), std::to_string(line), line);
		}
	}

	inline int run_tests(std::ostream& os) {
		os << "Running "
				<< get_tests().size()
				<< " tests\n";

		int num_passed{0};
		int num_failed{0};
		int num_missed_failed{0};

		for(auto* test : get_tests()) {
			os << "---------------\n"
					<< test->get_name()
					<< '\n';
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
					os << "Missed expected failure\n"
						<< "Test passed but was expected to fail.\n";
				} else {
					++num_passed;
					os << "Passed\n";
				}
			} else if(!test->get_expected_reason().empty() && test->get_expected_reason() == test->get_reason()) {
				++num_passed;
				os << "Expected failure\n"
					<< test->get_reason()
					<< '\n';
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
		os << "---------------\n";
		os << "Tests passed: " << num_passed << '\n';
		os << "Tests failed: " << num_failed << '\n';
		if(num_missed_failed != 0) {
			os << "Tests failures missed: "
				<< num_missed_failed;
		}
		os << '\n';

		return num_failed;
	}
} // namespace ccunit

#define CCUNIT_CLASS_FINAL(line) Test ## line
#define CCUNIT_CLASS_RELAY(line) CCUNIT_CLASS_FINAL(line)
#define CCUNIT_CLASS CCUNIT_CLASS_RELAY(__LINE__)

#define CCUNIT_INSTANCE_FINAL(line) test ## line
#define CCUNIT_INSTANCE_RELAY(line) CCUNIT_INSTANCE_FINAL(line)
#define CCUNIT_INSTANCE CCUNIT_INSTANCE_RELAY(__LINE__)

#define TEST(test_name) namespace { \
	class CCUNIT_CLASS : public ccunit::TestBase { \
	public: \
		CCUNIT_CLASS(std::string_view name) : TestBase(name) {} \
		void run() override; \
	}; \
} /* unnamed namespace */ \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name);\
void CCUNIT_CLASS::run()

#define TEST_EX(test_name, exception_type) namespace { \
	class CCUNIT_CLASS : public ccunit::TestExBase<exception_type> { \
	public: \
		CCUNIT_CLASS(std::string_view name, std::string_view exception_name) : TestExBase(name, exception_name) {} \
		void run() override; \
	}; \
} /* unnamed namespace */ \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name, #exception_type);\
void CCUNIT_CLASS::run()

#define CONFIRM_FALSE(actual) ccunit::confirm(false, actual, __LINE__)
#define CONFIRM_TRUE(actual) ccunit::confirm(true, actual, __LINE__)
#define CONFIRM(expected, actual) ccunit::confirm(expected, actual, __LINE__)

#endif // CCUNIT_H
