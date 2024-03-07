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
	private:
		std::string name_;
		bool passed_;
		std::string reason_;
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
		for(auto* test : get_tests()) {
			os << "---------------\n"
					<< test->get_name()
					<< '\n';
			try {
				test->run_ex();
			} catch(...) {
				test->set_failed("Unexpected exception thrown.");
			}

			if(test->get_passed()) {
				++num_passed;
				os  << "Passed\n";
			} else {
				++num_failed;
				os << "Failed\n"
						<< test->get_reason()
						<< '\n';
			}
		}
		os << "---------------\n";
		if(num_failed == 0) {
			os << "All tests passed.\n";
		} else {
			os << "Tests passed: " << num_passed << '\n';
			os << "Tests failed: " << num_failed << '\n';
		}
		return num_failed;
	}
} // namespace ccunit

#define CCUNIT_CLASS_FINAL(line) Test ## line
#define CCUNIT_CLASS_RELAY(line) CCUNIT_CLASS_FINAL(line)
#define CCUNIT_CLASS CCUNIT_CLASS_RELAY(__LINE__)

#define CCUNIT_INSTANCE_FINAL(line) test ## line
#define CCUNIT_INSTANCE_RELAY(line) CCUNIT_INSTANCE_FINAL(line)
#define CCUNIT_INSTANCE CCUNIT_INSTANCE_RELAY(__LINE__)

#define TEST(test_name) class CCUNIT_CLASS : public ccunit::TestBase { \
public: \
	CCUNIT_CLASS(std::string_view name) : TestBase(name) { \
		ccunit::get_tests().push_back(this); \
	} \
	void run() override; \
}; \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name);\
void CCUNIT_CLASS::run()

#define TEST_EX(test_name, exception_type) class CCUNIT_CLASS : public ccunit::TestBase { \
public: \
	CCUNIT_CLASS(std::string_view name) : TestBase(name) { \
		ccunit::get_tests().push_back(this); \
	} \
	void run_ex() override { \
		try { \
			run(); \
		} catch(exception_type const& e) { \
		\
		} \
	} \
	void run() override; \
}; \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name);\
void CCUNIT_CLASS::run()


#endif // CCUNIT_H
