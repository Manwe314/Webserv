#include "Webserv.hpp"


std::string str(size_t s)
{
    std::stringstream ss;

    ss << s;

    return ss.str();
}

int main()
{
    unsigned int host = 2130706433;
    
    std::cout << BLUE << str(host) << DEFAULT << std::endl;
    
    return 0;
}
