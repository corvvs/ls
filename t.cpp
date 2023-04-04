#include <iostream>
#include <typeinfo>

#include <iterator>
namespace ft = std;


class TraitsTest
{
  public:
    typedef int difference_type;
    typedef int value_type;
    typedef int pointer;
    typedef int reference;
    typedef int iterator_category;
};

int main()
{
    typedef ft::iterator_traits<TraitsTest> Traits;

    std::cout << (typeid(int) == typeid(Traits::difference_type)) << std::endl;
    std::cout << (typeid(int) == typeid(Traits::value_type)) << std::endl;
    std::cout << (typeid(int) == typeid(Traits::pointer)) << std::endl;
    std::cout << (typeid(int) == typeid(Traits::reference)) << std::endl;
    std::cout << (typeid(int) == typeid(Traits::iterator_category)) << std::endl;
}
