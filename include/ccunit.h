#ifndef CCUNIT_H
#define CCUNIT_H

#include <iostream>
#include <string_view>
#include <vector>

namespace CCUnit
{
    class TestBase
    {
        private:
            std::string name;
            bool passed;
            std::string reason;
        public:
            TestBase (std::string_view name) : name(name), passed(true) { }
            virtual ~TestBase() = default;
            virtual void run () = 0;
            std::string_view getName () const
            {
                return name;
            }
            bool getPassed () const
            {
                return passed;
            }
            std::string_view getReason () const
            {
                return reason;
            }
            void setFailed(std::string_view reason)
            {
                this->passed = false;
                this->reason = reason;
            }
    };
    
    inline std::vector<TestBase *> &getTests()
    {
        static std::vector<TestBase *> tests;
        return tests;
    }
    inline void runTests()
    {
        for (auto *test : getTests())
        {
            std::cout << "---------------\n";
            std::cout << test->getName() << '\n';
            try
            {    
                test->run();
            }
            catch(...)
            {
                test->setFailed("Unexpected exception thrown.");
            }
            if (test->getPassed())
            {
                std::cout << "Passed\n";
            }
            else
            {
                std::cout << "Failed\n";
                std::cout << test->getReason() << '\n';
            }
        }
    }
} // namespace CCUnit

#define TEST class Test : public CCUnit::TestBase \
{ \
    public: \
        Test(std::string_view name) : TestBase(name) \
        { \
            CCUnit::getTests().push_back(this); \
        } \
        void run () override; \
}; \
Test test("testCanBeCreated"); \
void Test::run()

#endif // CCUNIT_H