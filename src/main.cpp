#include "ccunit.h"
#include <iostream>

TEST
{
    std::cout << name << '\n';
}

int main()
{
    CCUnit::runTests();
    return 0;
}