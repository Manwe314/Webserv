/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 21:44:05 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/04 14:27:48 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

const char * HostDeclarationIssue::what() const throw()
{
    return ("IPv4, port address declaration inccorect");
}

const char * BracketPairMissMatch::what() const throw()
{
    return (msg.c_str());   
}


t_host_port ServerConfig::initHostPort(std::vector<std::string> server_block)
{
    std::vector<std::string>::iterator it;
    std::vector<std::string> array;
    t_host_port pair;
    unsigned int host;
    /*
        Find 'listen' and consider the next input.
		If listen isn't declared, then HostDeclarationIssue will be thrown.
        Split it on . and : to have a vector of 5 elemnents (4 octets and port).
        Populate the struct by combining the 4 octets into one unsigned int
		and stocking the port as an int
    */
    it = std::find(server_block.begin(), server_block.end(), "listen");
    if (it == server_block.end())
        throw HostDeclarationIssue();
    else
    {
        it++;
        array = split(*it, ".:");
		/*
        	If the declaration used localhost convert it to 127.0.0.1
			and place it before the port in the array.
		*/
        if (array[0] == "localhost")
        {
            array.erase(array.begin());
            array.insert(array.begin(), "127");
            array.insert(array.begin() + 1, "0");
            array.insert(array.begin() + 2, "0");
            array.insert(array.begin() + 3, "1");
        }
        if (array.size() != 5)
            throw HostDeclarationIssue();
        /*
			Make sure that each octet has a value betwwen 0 and 255,
			take the least significant 8 bits (first 8) of each octet
			and place it in order inside "host" unsigned int.
		*/
        host = 0;
        for (int i = 0; i < 4; i++)
        {
            if (std::atoi(array[i].c_str()) > 255 || std::atoi(array[i].c_str()) < 0)
                throw HostDeclarationIssue();
            host = (host << 8) | (std::atoi(array[i].c_str()) & 0xFF);
        }
        //Make sure that the port number is between 0 and max 16 bit number
        if (std::atoi(array[4].c_str()) < 0 || std::atoi(array[4].c_str()) > 65535)
            throw HostDeclarationIssue();
        pair.host = host;
        pair.port = std::atoi(array[4].c_str());
    }
    // std::cout << BLUE << "HOST: " << DEFAULT << pair.host << BLUE << " PORT: " \
	// << DEFAULT << pair.port << std::endl;
    return (pair);
}

std::string ServerConfig::initServerName(std::vector<std::string> server_block)
{
    std::vector<std::string>::iterator it;
    std::string server_name;
	/*
    	Find where server_name is declared, the next element will be the name for the server.
    	If in the config server, the server_name was not provided but the server was declared,
		the name will be empty
    	Next deliminated text will be considered as a name. eg:
        
		server_name
        listen localhost:8000
        in this case the server name will be "listen"
    */
    it = std::find(server_block.begin(), server_block.end(), "server_name");
    if (it == server_block.end())
        server_name = "";
    else
        server_name = *(++it);
    // std::cout << BLUE << "Server Name " << DEFAULT << server_name << std::endl;
    return server_name;
}

ServerRoutesConfig ServerConfig::initServerWideConfig(std::vector<std::string> server_block)
{
    std::pair<int, int> brackets;
    std::vector<std::string>::iterator it;
    /*
    	A loop to find all declarations of subroutes.
		Removing them only leaves serverwide configurations.
	*/
    while (true)
    {
        //printVector(server_block);
        it = std::find(server_block.begin(), server_block.end(), "location");
        //If no more "location's are found then all of them have been removed. Thus, we are done. 
        if (it == server_block.end())
            break;
        /*
			This will return the largest encapsulation of a sub route
			i.e the first "location" and the last "}" (the last bracket of this specific route).
		*/
        brackets = encapsule(server_block, "location", "}", std::distance(server_block.begin(), it));
        /*
        	Erase if such pair was found.
			In the case where "location" was found but the "}" bracket wasn't, nothing will happen.
        	If thats the case that means brackets are missmatched,
			therefore, the BracketPairMissMatch will be thrown later on.
		*/
        if (brackets.second != -1) {
            eraseRange(server_block, brackets.first, brackets.second);
		}
        else
            break;
    }
	/*
    	What is left after the loop is everything outside "location"-"}" pairs
		and the encapsulating "{ }"s
    	To make sure of this, a pair of "{ }" are looked for inside the server brackets.
		If any are found there was a missmatch. 
	*/
    brackets = encapsule(server_block, "{", "}", 1);
    
    
    if (brackets.second != -1)
        throw BracketPairMissMatch("Error in " + _server_name + " Brackets are missmatched");
    return (ServerRoutesConfig(server_block, ""));
}

std::vector<ServerRoutesConfig> ServerConfig::initRouteConfigs(std::vector<std::string> server_block)
{
    std::pair<int, int> brackets;
    std::vector<std::string> buffer;
    std::vector<std::string>::iterator it;
    std::vector<ServerRoutesConfig> route_configs;
    int offset = 0;
    
	/*
    	Loop to find all route blocks identified by the "location" rule.
    	Use the offset int to achive this and make the vector of route configs
	*/
    while (true)
    {
        //In the begining offset is 0, from there find the first occurance of "location".
        it = std::find(server_block.begin() + offset, server_block.end(), "location");
        //This if statement will eventually break the loop when "location" is no longer found.
        if (it == server_block.end())
            break;
        /*
			Get a pair of indexes for the curly brackets
			that normally follow the path after the location rule.
		*/
        brackets = encapsule(server_block, "{", "}", std::distance(server_block.begin(), it));
        /*
			If the brackets are present, create the route config object and add it to the vector.
			If not, discard this "location" statement.
        	For now this will just be ignored,
			although nginx throws a config error (this could be adapted).
        	Check if an empty range was returned.
		*/
        if (brackets.second != -1)
        {
            //Clear the buffer and reserve space for the necessary elements;
            buffer.clear();
            /*
				Start from the index of the first bracket until (including) the second
				and copy the contents into the buffer
			*/
            for (int i = brackets.first; i <= brackets.second; i++)
                buffer.push_back(server_block[i]);
			/*
            	The route block deliminated by curly brackets are passed to the constructor,
				along the location "path".
            	Only the NEXT element to the location is considered to be the "path".
				If there are more elements between "{" and "location",
            	everything other than the first element is ignored.
				If there is nothing inbetween, "{" is considered as the path.
			*/
            route_configs.push_back(ServerRoutesConfig(buffer, *(++it)));
            //Move the offset to the second bracket such that sub-route locations are skipped.
            offset = brackets.second;
        }
        else
            offset = std::distance(server_block.begin(), ++it);
    }
    return (route_configs);
}

ServerConfig::ServerConfig(std::vector<std::string> server_block) : _pair(initHostPort(server_block)),\
                                                                     _server_name(initServerName(server_block)),\
                                                                      _serverwide_config(initServerWideConfig(server_block)),\
                                                                       _route_configs(initRouteConfigs(server_block))
{
    if (TESTING)
    {
        std::cout << "Server: " << _server_name << " Config Created!" << std::endl;
    }
       
}

ServerConfig::ServerConfig()
{
    _pair.host = 2130706433;
    _pair.port = 8080;
    _server_name = "";
}

ServerConfig::~ServerConfig()
{
    
}

ServerConfig& ServerConfig::operator=(const ServerConfig& rhs)
{
    _pair = rhs.getHostPortPair();
    _server_name = rhs.getName();
    _serverwide_config = rhs.getServerWideConfig();
    _route_configs = rhs.getRouteConfigs();
   return *this; 
}

t_host_port ServerConfig::getHostPortPair() const
{
    return (_pair);    
}
std::string ServerConfig::getName() const
{
    return (_server_name);
}
ServerRoutesConfig ServerConfig::getServerWideConfig() const
{
    return (_serverwide_config);        
}
std::vector<ServerRoutesConfig> ServerConfig::getRouteConfigs() const
{
    return (_route_configs);        
}
ServerRoutesConfig ServerConfig::getRouteConfigByLocation(std::string location) const
{
    if (_route_configs.empty())
        throw InvalidGetCall("There are no routes in the server named: " + _server_name);
    for (std::vector<ServerRoutesConfig>::const_iterator i = _route_configs.begin(); \
	i != _route_configs.end(); i++)
        if ((*i).getLocation() == location)
            return (*i);
    throw InvalidGetCall("There are no routes with matching location to " + location);
    return (_route_configs[0]);
}
