#ifndef CCUNIT_H
#define CCUNIT_H

#include <ostream>
#include <string_view>
#include <vector>

namespace ccunit {
	class TestBase {
	public:
		TestBase(std::string_view name) : name_(name), passed_(true) {}
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
		void set_failed(std::string_view reason) {
			passed_ = false;
			reason_ = reason;
		}
		std::string_view get_expected_reason() const {
			return expected_reason_;
		}
		void set_expected_failure_reason(std::string_view reason) {
			expected_reason_ = reason;
		}
	private:
		std::string name_;
		bool passed_;
		std::string reason_;
		std::string expected_reason_;
	};

	class ConfirmException {
	public:
		ConfirmException() = default;
		virtual ~ConfirmException() = default;
		std::string_view get_reason() const {
			return reason_;
		}
	protected:
		std::string reason_;
	};

	class BoolConfirmException : public ConfirmException {
	public:
		BoolConfirmException(bool expected, int line) {
			reason_ = "Confirm failed on line ";
			reason_ += std::to_string(line) + '\n';
			reason_ += "    Expected: ";
			reason_ += expected ? "true" : "false";
		}
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

	inline std::vector<TestBase*>& get_tests() {
		static std::vector<TestBase*> tests;
		return tests;
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
				test->set_failed(ex.get_reason());
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
				os << "Failed\n"
						<< test->get_reason()
						<< '\n';
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
		CCUNIT_CLASS(std::string_view name) : TestBase(name) { \
			ccunit::get_tests().push_back(this); \
		} \
		void run() override; \
	}; \
} /* unnamed namespace */ \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name);\
void CCUNIT_CLASS::run()

#define TEST_EX(test_name, exception_type) namespace { \
	class CCUNIT_CLASS : public ccunit::TestBase { \
	public: \
		CCUNIT_CLASS(std::string_view name) : TestBase(name) { \
			ccunit::get_tests().push_back(this); \
		} \
		void run_ex() override { \
			try { \
				run(); \
			} catch(exception_type const& e) { \
				return; \
			} \
			throw ccunit::MissingException(#exception_type); \
		} \
		void run() override; \
	}; \
} /* unnamed namespace */ \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name);\
void CCUNIT_CLASS::run()

#define CONFIRM_FALSE(actual) \
if(actual) { \
	throw ccunit::BoolConfirmException(false, __LINE__); \
}

#define CONFIRM_TRUE(actual) \
if(actual) { \
	throw ccunit::BoolConfirmException(true, __LINE__); \
}

#endif // CCUNIT_H
