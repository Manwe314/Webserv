/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 14:44:27 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/24 16:11:14 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

//C++ includes
#include<iostream>
#include<iomanip>
#include<string>
#include<cstdlib>
#include<cstring>
#include<stdexcept>
#include<algorithm>
#include<sstream>
#include<fstream>
#include<utility>
#include<cctype>
#include <sys/wait.h>

//C++ Container includes
#include<vector>
#include<list>
#include<map>
#include<stack>


//C includes
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<poll.h>
#include<cstdio>
#include<ctime>


//C Network includes
#include<arpa/inet.h>
#include<sys/socket.h>

//General Defines
#define PORT 9999
#define BUFFER_SIZE 1024
#define TESTING 1
#define REQUEST_MAX 1000
#define MAX_EVENTS 1000
#define TIMEOUT_SEC 2
#define DEFAULT_ERROR "/Users/brettleclerc/Documents/ecole_42/schoolWork/webserv_github_levan/error_pages"

//Colours for readbility
#define DEFAULT "\033[0m"
#define MAGENTA "\033[0;35m"
#define GREEN "\033[0;32m"
#define BLUE "\033[38;5;117m"
#define CYAN "\033[0;36m"
#define YELLOW "\033[0;33m"
#define LAVENDER "\033[38;5;183m"

//Structurtes
typedef struct s_host_port
{
    int port;
    unsigned int host;
}   t_host_port;



//Utility function declarations
std::string intToString(int num);
void toLowerCase(std::string &str);
std::string sizetToString(size_t num);
std::string readFile(std::string full_name);
std::string readBinaryFile(const std::string &path);
std::vector<std::string> split(std::string string, char delim);
std::vector<std::string> split(std::string string, std::string delim);
int countMatchingChars(std::string first, std::string second, int pos = 0);
void eraseRange(std::vector<std::string>& array, int start, int end, int flag = 0);
std::pair<int, int> encapsule(std::vector<std::string> array, std::string a, std::string b, int pos = 0);
std::string	removeComments( std::string const & config_file );
std::string	trimSpaces( std::string string );

template <typename T>
void printVector(const std::vector<T>& arr)
{
    typename std::vector<T>::const_iterator it;
    for (it = arr.begin(); it != arr.end(); it++)
        std::cout << MAGENTA << *it << DEFAULT << std::endl;
}

template <typename Keytype, typename Valuetype>
void printMap(const std::map<Keytype, Valuetype>& map)
{
    typename std::map<Keytype, Valuetype>::const_iterator it;
    for (it = map.begin(); it != map.end(); it++)
        std::cout << "Key: " << it->first << "Value: " << it->second << std::endl;
}

bool isFile(const std::string& path);
bool isValidHeader(std::string& header);
bool isValidFile(std::string& file_path);
bool isValidVersion(std::string& version);
bool isDirectory(const std::string& path);
bool isInvalidVersion(std::string& version);
bool isValidHttpMethod(std::string& method);
bool isInvalidHttpMethod(std::string& method);
bool isAllowed(const std::vector<std::string>& allowed_methods, std::string &method);
bool isValidCgiFile( std::string const & file );

#endif
