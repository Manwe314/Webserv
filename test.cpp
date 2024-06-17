#include "Webserv.hpp"


int main()
{
    std::time_t raw_time;
    std::time(&raw_time);

    std::tm* gmt_time = std::gmtime(&raw_time);

    char  buffer[80];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt_time);
    
    std::cout << YELLOW << std::string(buffer) << std::endl;
    return 0;
}
