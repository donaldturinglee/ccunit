#ifndef CCUNIT_H
#define CCUNIT_H

#include <string_view>
#include <vector>

namespace ccunit {
	class TestInterface {
	public:
		virtual ~TestInterface() = default;
		virtual void run() = 0;
	};

	inline std::vector<TestInterface*>& get_tests() {
		static std::vector<TestInterface*> tests;
		return tests;
	}
	inline void run_tests() {
		for(auto* test : get_tests()) {
			test->run();
		}
	}
} // namespace ccunit

#define TEST class Test : public ccunit::TestInterface { \
public: \
	Test(std::string_view name) : name_(name), result_(true) { \
		ccunit::get_tests().push_back(this); \
	} \
	void run() override; \
private: \
	std::string name_; \
	bool result_; \
}; \
Test test("test_created");\
void Test::run()



#endif // CCUNIT_H
