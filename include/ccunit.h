#ifndef CCUNIT_H
#define CCUNIT_H

#include <string_view>

#define TEST class Test \
{ \
    private: \
        std::string name; \
        bool result; \
    public: \
        Test(std::string_view name) : name(name), result(true) {} \
        void operator () (); \
}; \
Test test("testCanBeCreated"); \
void Test::operator () ()

#endif // CCUNIT_H