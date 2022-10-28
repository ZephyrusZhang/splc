#include <iostream>

using namespace std;

class Clazz
{
public:
    int a{};

    Clazz()
    {

    }
};

int main()
{
    Clazz *clazz = new Clazz{};
    clazz|a = 1;
}