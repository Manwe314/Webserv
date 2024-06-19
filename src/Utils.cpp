/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 18:42:38 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/19 03:03:46 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Config.hpp"

/*
	Function to split a string based on a single char or a charset (multiple chars),
	returning a vector of strings that are ordered.
*/
std::vector<std::string> split(std::string string, char delim)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(string);
    
    while (std::getline(stream, token, delim))
        tokens.push_back(token);
    tokens.erase(std::remove(tokens.begin(), tokens.end(), ""), tokens.end());
    return tokens;
}

std::vector<std::string> split(std::string string, std::string delim)
{
    std::vector<std::string> tokens;
    std::string token;
    std::string::size_type start;
    std::string::size_type end = 0;

    if (delim.empty())
    {
        tokens.push_back(string);
        return (tokens);
    }

    string += delim[0];
    
    while (true)
    {
        start = string.find_first_not_of(delim, end);
        if (start == std::string::npos)
            break;
        end = string.find_first_of(delim, start);
        if (end == std::string::npos)
            end = string.size() - 1;
        token = string.substr(start, end - start);
        tokens.push_back(token);
    }
    return (tokens);
}

/*
	This function returns a pair of ints that points to 2 elements in a vector.
	
	First will point to the element matching 'a',
	second will point to the element matching 'b'.
	
	The position can be specified as to where to start finding this pair.
	Additionally this a, b pair is found with encapsulation in mind.
	
	Eg:
		a = " and b = '.
		The input vector elements are: ", hello, ", world, ', !, '.
		If pos = 0 then the function will return the pair with first "  and LAST '
		since they are on the same "level".
		If pos = 1 then the function will return the pair with second "  and FIRST '
		since they are on the same "level".
*/
std::pair<int, int> encapsule(std::vector<std::string> array, std::string a, std::string b, int pos)
{
    int start = -1;
    int end = -1;
    int i = pos;
	/*
    	This int will track the level we are at.
		The first 'a' we encounter will be considered as level 0.
	*/
    int level = 0;

	/*
    	If the starting pos is out of range, a pair containing at least one -1 will be returned.
    	Check that pos is not out of range.
	*/
    if (i >= (int)array.size())
        return (std::make_pair(start, end));

    while (array[i] != a && i < (int)array.size())
        i++;
	//If 'a' is not found at all a pair containing at least one -1 will be returned.
    if (i == (int)array.size())
        return (std::make_pair(start, end));
	//The first 'a' that IS found is considered as the start or first element of the pair.
    start = i;
    //std::cout << *start << " at " << std::distance(array.begin(), start) << std::endl;
    i++;
    while (i < (int)array.size())
    {
        //Every other 'a' encountered before b-s increases the level.
        if (array[i] == a)
            level++;
		/*
        	If the level is not 0, I.E. more than 1 'a' was encountered,
			then 'b' only decreases the level.
		*/
        if (array[i] == b && level != 0)
            level--;
        else if (array[i] == b && level == 0)
            break;
        //If the level is 0, the pair has been found.
        i++;
    }
    //If 'a' IS found but 'b' is NOT, then a pair of end iterators are returned.
    if (i == (int)array.size() && array[i] != b)
        return (std::make_pair(start, end));
    end = i;
    //std::cout << *end << "  ending at " << std::distance(array.begin(), end) << std::endl;
    return (std::make_pair(start, end));
}

/*
	Erase a range identified by indexes (ints) similar to .erase().
	Flag by default is set to 0, therefore, it is an all inclusive deletion.
	If the flag is anything but 0, then the last (end) element will not be erased.
*/
void eraseRange(std::vector<std::string>& array, int start, int end, int flag)
{
    if (start >= (int)array.size() || end >= (int)array.size())
        return ;

    std::vector<std::string>::iterator st = array.begin() + start;
    std::vector<std::string>::iterator en = array.begin() + end;

    array.erase(st, en);

    if (flag == 0)
    {   
        en = array.begin() + start;
        if (en != array.end())
            array.erase(en);    
    }
}

//this function puts the given string to lowercase letters
void toLowerCase(std::string &str)
{
    for (std::string::iterator it = str.begin(); it != str.end(); it++)
    {
        if (std::isalnum(*it))
            *it = std::tolower(*it);
    }
}


//this function count how many characters match until it doesnt starting from pos (default 0), between first and second strings.
int countMatchingChars(std::string first, std::string second, int pos)
{
    int count;
    int min_size;

    count = 0;
    min_size = std::min(first.size(), second.size());
    
    if (pos > min_size)
        return (-1);
    for (int i = pos; i < min_size; i++)
    {
        if (first[i] == second[i])
            count++;
        else
            break;
    }
    return (count);
}

//Function to read an entire file into a single string (including new line characters).
std::string readFile(std::string full_name)
{
    //Open file, make a string stream object and a string variable to read lines into.
    std::ifstream file(full_name.c_str());
    std::ostringstream string_stream;
    std::string line;
    
    //If the file cant be opened throw an exception.
    if (!file.is_open())
        throw CouldNotOpenFile();
    /*
    	Read while there are lines to read and build up the string stream object
		line by line making sure to add newline characters
	*/
    while (std::getline(file, line))
        string_stream << line << '\n';
    //Close the file    
    file.close();

    //Trunc the object into a string and return it.
    return (string_stream.str());
}

//this fucntion converts an int to a string
std::string intToString(int num)
{
    std::ostringstream oss;

    oss << num;

    return (oss.str());
}

//this fucntion converts an size_T to a string
//i know about templates, but i dont want random data types causing problems here.
std::string sizetToString(size_t num)
{
    std::ostringstream oss;

    oss << num;

    return (oss.str());
}

std::string readBinaryFile(const std::string &path)
{
    std::string data;
    std::ifstream file(path.c_str(), std::ios::binary);

    if (!file.is_open())
        throw CouldNotOpenFile();
    
    file.seekg(0, std::ios::end);
    data.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&data[0], data.size());
    return (data);
}


//this function returns true if a header line is formated correctly or false if its not.
//only rule is that the string must start with a name that is between 33 and 126 asccii characters ending with a colon ":". after that its free game.
bool isValidHeader(std::string& header)
{
    size_t i;
    
    i = header.find(":");
    if (i == std::string::npos)
        return (false);
    for (size_t j = 0; j < i; j++)
    {
        if (header[i] < 33 || header[i] > 126)
            return (false);
    }
    return (true);
}

/*
	This function returns true if the given string is a valid http method
	or false if it is anything else.
*/
bool isValidHttpMethod(std::string& method)
{
	/*
    	ConfigBase's member variable _http_methods
		contains a complete list of VALID http methods.
		Check against it to veryfiy the argument.
	*/
    for (std::vector<std::string>::const_iterator it = ConfigBase::_http_methods.begin(); it != ConfigBase::_http_methods.end(); it++)
        if(method.compare(*it) == 0)
            return (true);
    return (false);
}

/*
	This function returns true if the given string is an invalid (unsupported) http method,
	false if it's anything else.
*/
bool isInvalidHttpMethod(std::string& method)
{
    if (method.compare("OPTIONS") == 0 || method.compare("HEAD") == 0)
        return (true);
    if (method.compare("TRACE") == 0 || method.compare("CONNECT") == 0)
        return (true);
    return (false);
}


/*
	This function returns true if the given string is a supported http version,
	or false if it's anything else.
*/
bool isValidVersion(std::string& version)
{
    if (version.compare("HTTP/1.0") == 0 || version.compare("HTTP/1.1") == 0)
        return (true);
    return (false);
}

/*
	This function returns true if the given string is a non supported http version,
	or false if it's anything else;
*/
bool isInvalidVersion(std::string& version)
{
    if (version.compare("HTTP/0.9") == 0 || version.compare("HTTP/3.0") == 0 || version.compare("HTTP/2.0") == 0)
        return (true);
    return (false);
}

/*
    this function returns true if the given file path points to a valid, readable file
    or false if it dosent.
*/
bool isValidFile(const std::string& file_path)
{
    std::ifstream file(file_path.c_str());
    return file.is_open();
}

/*
    this function returns true if a given path point to a directory
    or false if it doesnt or if path is invalid.
*/
bool isDirectory(const std::string& path)
{
    struct stat stat_buffer;
    if (stat(path.c_str(), &stat_buffer) != 0)
        return (false);
    return (S_ISDIR(stat_buffer.st_mode));
}

/*
    this function returns true if a given path points to a file
    or false if it doesnt or if path is invalid
*/
bool isFile(const std::string& path)
{
    struct stat stat_buffer;
    if (stat(path.c_str(), &stat_buffer) != 0)
        return (false);
    return (S_ISREG(stat_buffer.st_mode));
}

/*
    this function returns true if the method string is in the allow_methods vector (return true if vector is empty)
    or false if it isnt.
*/
bool isAllowed(const std::vector<std::string>& allowed_methods, std::string &method)
{
    if (allowed_methods.empty())
        return (true);
    for (std::vector<std::string>::const_iterator i = allowed_methods.begin(); i != allowed_methods.end(); i++)
        if ((*i).compare(method) == 0)
            return (true);
    return (false);
}


std::ostream& operator<<(std::ostream& obj, const s_host_port& pair)
{
    unsigned int one = pair.host & 0xFF;
    unsigned int two = (pair.host >> 8) & 0xFF;
    unsigned int three = (pair.host >> 16) & 0xFF;
    unsigned int four = (pair.host >> 24) & 0xFF;
    obj << "HOST PORT PAIR: ";
    obj << four;
    obj << ".";
    obj << three;
    obj << ".";
    obj << two;
    obj << ".";
    obj << one;
    obj << ":";
    obj << pair.port;
    return (obj);
}

static void printRouteConfig(std::vector<ServerRoutesConfig> routes)
{
    if (!routes.empty())
        {
            for (std::vector<ServerRoutesConfig>::iterator i4 = routes.begin(); i4 != routes.end(); i4++)
            {
                std::cout << MAGENTA << "--ROUTE--" << DEFAULT << std::endl;
                std::cout << "Root: " << (*i4).getRoot() << std::endl;
                std::cout << "Location: " << (*i4).getLocation() << std::endl;
                std::cout << "Index Files:";
                std::vector<std::string> vec2 = (*i4).getIndex();
                if (!vec2.empty())
                {
                    for (std::vector<std::string>::iterator i5 = vec2.begin(); i5 != vec2.end(); i5++)
                        std::cout << " " << *i5 << ",";
                }
                std::cout << std::endl;
                std::cout << "Allowed methods:";
                std::vector<std::string> vect2 = (*i4).getMethods();
                if (!vect2.empty())
                {
                    for (std::vector<std::string>::iterator i6 = vect2.begin(); i6 != vect2.end(); i6++)
                        std::cout << " " << *i6 << ",";
                }
                std::cout << std::endl;
                std::vector<ServerRoutesConfig> sub = (*i4).getSubRoutes();
                if (!sub.empty())
                {
                    std::cout << LAVENDER << "+SUB+" << DEFAULT << std::endl;
                    printRouteConfig(sub);
                    std::cout << LAVENDER << "+++++" << DEFAULT << std::endl;
                }
                std::cout << MAGENTA << "---------" << DEFAULT << std::endl;
            }
        } 
}



//this clusterfuck of a function prints the contents of the config object so all the configuration taken from the config file (it is currantly up to date with functionalty).
void printConfig(Config conf)
{
    std::vector<ServerConfig> servers = conf.getServerConfigs();
        
    
    std::cout << "~~~~~~~~~~~~~~~~~" << GREEN << "THE CONFIG" << DEFAULT << "~~~~~~~~~~~~~~~~~" << std::endl;
    for (std::vector<ServerConfig>::iterator i = servers.begin(); i != servers.end(); i++)
    {
        std::vector<t_host_port> pair = (*i).getHostPortPair();
        std::cout << GREEN << "_____________________________________________" << DEFAULT << std::endl;
        std::cout << BLUE << "SERVER NAME: " << DEFAULT << (*i).getName() << std::endl;
		printVector(pair);
        std::cout << "----" << CYAN << "SERVERWIDE CONFIG" << DEFAULT << "----" << std::endl;
        ServerRoutesConfig serverwide = (*i).getServerWideConfig();
        std::cout << CYAN << "Root: " << DEFAULT << serverwide.getRoot() << std::endl;
        std::cout << CYAN << "Location: " << DEFAULT << serverwide.getLocation() << std::endl;
        std::cout << CYAN << "Index Files: " << DEFAULT;
        std::vector<std::string> vec = serverwide.getIndex();
        if (!vec.empty())
        {
            for (std::vector<std::string>::iterator i2 = vec.begin(); i2 != vec.end(); i2++)
                std::cout << " " << *i2 << ",";
        }
        std::cout << std::endl;
        std::cout << CYAN << "Allowed methods: " << DEFAULT;
        std::vector<std::string> vect = serverwide.getMethods();
        if (!vect.empty())
        {
            for (std::vector<std::string>::iterator i3 = vect.begin(); i3 != vect.end(); i3++)
                std::cout << " " << *i3 << ",";
        }
        std::cout << std::endl;
        std::cout << "-------------------------" << std::endl;
        std::vector<ServerRoutesConfig> routes = (*i).getRouteConfigs();
        printRouteConfig(routes);
        std::cout << "_____________________________________________" << std::endl;
    } 
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
}
