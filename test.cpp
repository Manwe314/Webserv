#include "Webserv.hpp"

int main()
{
    std::vector<std::string> vec = {"hi", "bob", "alice", "hi", " bye"};
    std::string a = "alice";

    for (std::vector<std::string>::iterator it1 = vec.begin(); it1 != vec.end(); it1++)
    {
        if (*it1 == a)
            std::cout << "found " << *it1 << " the index as by distance: " << std::distance(vec.begin(), it1) << std::endl;        
    }
    

    return 0;
}