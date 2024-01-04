#include "ccunit.h"

// TEST
// {
//     std::cout << getName() << '\n';
// }

CCU("Test can be created")
{

}

CCU_EX("Test with throw can be created", int)
{
    throw 1;
}