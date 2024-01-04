#ifndef CCUNIT_H
#define CCUNIT_H

#include <ostream>
#include <string_view>
#include <vector>

namespace CCUnit
{
    class Test_Base
    {
        private:
            std::string name;
            bool passed;
            std::string reason;
        public:
        Test_Base (std::string_view name) : name{name}, passed{true} { }
            virtual ~Test_Base() = default;
            virtual void run_exception ()
            {
                run();
            }
            virtual void run () = 0;
            std::string_view get_name () const
            {
                return name;
            }
            bool get_passed () const
            {
                return passed;
            }
            std::string_view get_reason () const
            {
                return reason;
            }
            void set_failed(std::string_view reason)
            {
                this->passed = false;
                this->reason = reason;
            }
    };
    
    inline std::vector<Test_Base *> &get_tests()
    {
        static std::vector<Test_Base *> tests;
        return tests;
    }
    inline int run_tests(std::ostream &output)
    {
        output << "Running " << get_tests().size() << " tests\n";
        int num_passed {0};
        int num_failed {0};
        for (auto *test : get_tests())
        {
            output << "---------------\n";
            output << test->get_name() << '\n';
            try
            {    
                test->run_exception();
            }
            catch(...)
            {
                test->set_failed("Unexpected exception thrown.");
            }
            if (test->get_passed())
            {
                ++num_passed;
                output << "Passed\n";
            }
            else
            {
                ++num_failed;
                output << "Failed\n";
                output << test->get_reason() << '\n';
            }
        }
        output << "---------------\n";
        if (num_failed == 0)
        {
            output << "All tests passed.\n";
        }
        else
        {
            output << "Tests passed: " << num_passed << '\n';
            output << "Tests failed: " << num_failed << '\n';
        }
        return num_failed;
    }
} // namespace CCUnit

#define CCUNIT_CLASS_FINAL( line ) Test ## line
#define CCUNIT_CLASS_RELAY( line ) CCUNIT_CLASS_FINAL( line )
#define CCUNIT_CLASS CCUNIT_CLASS_RELAY( __LINE__ )
#define CCUNIT_INSTANCE_FINAL( line ) test ## line
#define CCUNIT_INSTANCE_RELAY( line ) CCUNIT_INSTANCE_FINAL( line )
#define CCUNIT_INSTANCE CCUNIT_INSTANCE_RELAY( __LINE__ )

#define CCU( test_name ) \
class CCUNIT_CLASS : public CCUnit::Test_Base \
{ \
    public: \
        CCUNIT_CLASS (std::string_view name) : Test_Base{name} \
        { \
            CCUnit::get_tests().push_back(this); \
        } \
        void run () override; \
}; \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name); \
void CCUNIT_CLASS::run()

#define CCU_EX( test_name, exception_type ) \
class CCUNIT_CLASS : public CCUnit::Test_Base \
{ \
    public: \
        CCUNIT_CLASS (std::string_view name) : Test_Base{name} \
        { \
            CCUnit::get_tests().push_back(this); \
        } \
        void run_exception () override \
        { \
            try \
            { \
                run(); \
            } \
            catch (exception_type const &) \
            { \
            } \
        } \
        void run () override; \
}; \
CCUNIT_CLASS CCUNIT_INSTANCE(test_name); \
void CCUNIT_CLASS::run()
#endif // CCUNIT_H