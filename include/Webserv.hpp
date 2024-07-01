/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 14:44:27 by lkukhale          #+#    #+#             */
/*   Updated: 2024/07/01 21:42:22 by lkukhale         ###   ########.fr       */
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
#include<dirent.h>


//C Network includes
#include<arpa/inet.h>
#include<sys/socket.h>

//General Defines
#define PORT 9999
#define BUFFER_SIZE 1024
#ifndef DEBUG
    #define DEBUG 1
#endif
#define REQUEST_MAX 1000
#define MAX_EVENTS 1000
#define TIMEOUT_SEC 2
#define DEFAULT_ERROR "./error_pages"
#define SAFE_MODE 1
#define MAX_BODY_SIZE 15000000

//Colours for readbility
#define DEFAULT "\033[0m"
#define MAGENTA "\033[0;35m"
#define GREEN "\033[0;32m"
#define BLUE "\033[38;5;117m"
#define CYAN "\033[0;36m"
#define YELLOW "\033[0;33m"
#define LAVENDER "\033[38;5;183m"

//Structurtes and Enums
typedef struct s_host_port
{
    int port;
    unsigned int host;
    bool operator==(const s_host_port& other) const {
        return (port == other.port && host == other.host);
    }
}   t_host_port;

std::ostream& operator<<(std::ostream& obj, const s_host_port& pair);

enum FileStatus{
    DOES_NOT_EXIST,
    PERMISSION_DENIED,
    IS_DIRECTORY,
    IS_FILE,
    UNKNOWN
};



//Utility function declarations
std::string intToString(int num);
void toLowerCase(std::string &str);
std::string sizetToString(size_t num);
std::string readFile(std::string full_name);
std::string lastModifiedTime(std::string path);
std::string readBinaryFile(const std::string &path);
std::vector<std::string> split(std::string string, char delim);
std::string listDirectoryContents(std::string uri, t_host_port pair);
std::vector<std::string> split(std::string string, std::string delim);
void eraseRange(std::vector<std::string>& array, int start, int end, int flag = 0);
int countMatchingChars(std::string first, std::string second, int pos = 0, int flag = 0);
std::pair<int, int> encapsule(std::vector<std::string> array, std::string a, std::string b, int pos = 0);
std::string	removeComments( std::string const & config_file );
std::string	trimSpaces( std::string string );
void	freeCsplit(char **array);
int		cArraySize(char **array);
char    *ft_strdup(char const *s1);

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

int pathStatus(const std::string& path);

bool isValidHeader(std::string& header);
bool isValidVersion(std::string& version);
bool isValidFile(const std::string& path);
bool isInvalidVersion(std::string& version);
bool isValidHttpMethod(std::string& method);
bool isInvalidHttpMethod(std::string& method);
bool isAllowed(const std::vector<std::string>& allowed_methods, std::string &method);
bool isValidCgiFile( std::string const & file );
bool isAllPaired(std::vector<std::string> array);

#endif
