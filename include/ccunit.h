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

#define CCUNIT_CLASS_FINAL( line ) Test ## line
#define CCUNIT_CLASS_RELAY( line ) CCUNIT_CLASS_FINAL( line )
#define CCUNIT_CLASS CCUNIT_CLASS_RELAY( __LINE__ )
#define CCUNIT_INSTANCE_FINAL( line ) test ## line
#define CCUNIT_INSTANCE_RELAY( line ) CCUNIT_INSTANCE_FINAL( line )
#define CCUNIT_INSTANCE CCUNIT_INSTANCE_RELAY( __LINE__ )

#define TEST( test_name ) \
class CCUNIT_CLASS : public CCUnit::TestBase \
{ \
    public: \
        CCUNIT_CLASS (std::string_view name) : TestBase(name) \
        { \
            CCUnit::getTests().push_back(this); \
        } \
        void run () override; \
}; \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name); \
void CCUNIT_CLASS::run()

#endif // CCUNIT_H