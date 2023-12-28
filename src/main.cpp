#include <iostream>
#include <string_view>

class Test
{
    private:
        std::string name;
        bool result;
    public:
        Test(std::string_view name) : name(name), result(true) {}
        void operator () ()
        {
            std::cout << name << '\n';
        }
};

Test test("testCanBeCreated");

int main()
{
    test();
    return 0;
}