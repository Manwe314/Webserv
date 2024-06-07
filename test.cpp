#include "Webserv.hpp"

class A
{
private:
public:
    int num;
    std::vector<A> nest;
    struct s_a find(int n);
    A(int n);
    ~A();
};

struct s_a
{
    A* it;
};

A::A(int n) : nest()
{

    num = n * 2;
    if (n < 6)
    {
        for (int i = 1; i < 4; i++)
        {
            nest.push_back(A(n + i));
        }
    }
}

A::~A()
{

}

struct s_a A::find(int n)
{
    struct s_a found;
    for (std::vector<A>::iterator i = nest.begin(); i != nest.end(); i++)
    {
        if (n == (*i).num)
        {
            found.it = this;
            return found;
        }
    }
    return found;
}

void fpi(struct s_a f)
{
    A* it = f.it;

    std::cout << "found " << (*it).num << std::endl;
}


int main()
{
    A a(2);
    struct s_a number = a.find(12);
    fpi(number);

    return 0;
}
