#include "Webserv.hpp"


int countMatchingChars(std::string first, std::string second, int pos, int flag)
{
    int count;
    int temp;
    int min_size;

    count = 0;
    temp = 0;
    min_size = std::min(first.size(), second.size());
    if (flag != 0)
    {
        
        if (pos > min_size)
            return (-1);
        for (int i = pos; i < min_size; i++)
        {
            if (first[i] == second[i])
                count++;
            else
                break;
        }
    }
    else
    {
        if (pos > min_size)
            return (-1);
        int i = pos;
        while (i < min_size)
        {
            if (first[i] == second[i])
            {
                temp++;
                if (first[i] == '/')
                    count = temp;
                if (i + 1 == min_size && first.size() == second.size())
                    count = temp;
                else if (i + 1 == min_size && first.size() > second.size())
                {
                    if (first[i + 1] == '/')
                        count = temp;
                }
                else if (i + 1 == min_size && first.size() < second.size())
                {
                    if (second[i + 1] == '/')
                        count = temp;
                }
            }
            else
                break;
            i++;
        }
    }
    
    return (count);
}

int main()
{
    std::string a = "/hello/qverty/r/as/d";
    std::string b = "/hello/qverty/r/as/d";

    int c = countMatchingChars(a, b);

    std::cout << BLUE << c << DEFAULT << std::endl;
    std::cout << LAVENDER << a[c -1] << " " << b[c -1] << DEFAULT << std::endl;



    return 0;
}
