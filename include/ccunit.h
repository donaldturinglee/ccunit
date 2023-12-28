#ifndef CCUNIT_H
#define CCUNIT_H

#include <string_view>
#include <vector>

namespace CCUnit
{
    class TestInterface
    {
        public:
            virtual ~TestInterface () = default;
            virtual void run () = 0;
    };
    inline std::vector<TestInterface *> &getTests()
    {
        static std::vector<TestInterface *> tests;
        return tests;
    }
    inline void runTests()
    {
        for (auto *test : getTests())
        {
            test->run();
        }
    }
} // namespace CCUnit

#define TEST class Test : public CCUnit::TestInterface \
{ \
    private: \
        std::string name; \
        bool result; \
    public: \
        Test(std::string_view name) : name(name), result(true) \
        { \
            CCUnit::getTests().push_back(this); \
        } \
        void run () override; \
}; \
Test test("testCanBeCreated"); \
void Test::run()

#endif // CCUNIT_H