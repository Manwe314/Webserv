#include "Webserv.hpp"

int main()
{
    std::map<std::string, std::string> mymap;

    mymap.insert(std::make_pair("hi", "henlo"));

    mymap["hi"] += " , I AM A GLODEN GOD!";

    std::cout << mymap["hi"] << std::endl;

    return 0;
}
