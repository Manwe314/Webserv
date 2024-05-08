/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 14:44:27 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/03 16:06:43 by lkukhale         ###   ########.fr       */
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

//C++ Container includes
#include<vector>
#include<list>
#include<map>


//C includes
#include<unistd.h>
#include<fcntl.h>
#include<poll.h>


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


//Structurtes
typedef struct s_host_port
{
    int port;
    unsigned int host;
}   t_host_port;





//Utility function declarations
std::vector<std::string> split(std::string string, char delim);
std::vector<std::string> split(std::string string, std::string delim);
bool IsFirstAfterWhitespace(std::string string, int pos);
std::pair<int, int> encapsule(std::vector<std::string> array, std::string a, std::string b, int pos = 0);
void eraseRange(std::vector<std::string>& array, int start, int end, int flag = 0);
void printVector(std::vector<std::string> arr);
template <typename Keytype, typename Valuetype>
void printMap(const std::map<Keytype, Valuetype>& map)
{
    typename std::map<Keytype, Valuetype>::const_iterator it;
    for (it = map.begin(); it != map.end(); it++)
        std::cout << "Key: " << it->first << "Value: " << it->second << std::endl;
}






#endif
