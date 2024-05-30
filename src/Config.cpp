/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 21:53:46 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/30 17:31:17 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "ConfigBase.hpp"

const char * CouldNotOpenFile::what() const throw()
{
    return ("Could not open the config file");
}

/*
	This function creates a vector of server configs
	based on servers described in the config file
*/
std::vector<ServerConfig> Config::initServerConfigs(std::string path_to_config)
{
    std::pair<int, int> brackets;
    std::vector<ServerConfig> server_configs;
    std::vector<std::string> split_file;
    std::vector<std::string> buffer;
    std::vector<std::string>::iterator it;
    std::string entire_file;
    int offset = 0;

    //read the entire file into a single string
    entire_file = readFile(path_to_config);

    //split it on a charset of space, tab and newline.
    split_file = split(entire_file, " \n\t");
    /*
		This while loop is identical to ServerConfig's initRouteConfigs() function.
		Further description can be found there (ServerConfig.cpp line 126)
    */
	while (true)
    {
        it = std::find(split_file.begin() + offset, split_file.end(), "server");

        if (it == split_file.end())
            break;
        
        brackets = encapsule(split_file, "{", "}", std::distance(split_file.begin(), it));

        if (brackets.second != -1)
        {
            buffer.clear();
            buffer.reserve( brackets.second - brackets.first);

            for (int i = brackets.first; i <= brackets.second; i++)
                buffer.push_back(split_file[i]);
            
            server_configs.push_back(ServerConfig(buffer));
            offset = brackets.second;
        }
        else
            offset =  std::distance(split_file.begin(), ++it);
    }
    return (server_configs);
}

//Function to read an entire file into a single string (including new line characters).
std::string Config::readFile(std::string full_name)
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

std::vector<ServerConfig> Config::getServerConfigs() const
{
    return _server_configurations;
}

ServerConfig Config::getServerConfigByName(std::string name) const
{
    if (_server_configurations.empty())
        throw InvalidGetCall("There are no Server Configurations Present");
    for (std::vector<ServerConfig>::const_iterator i = _server_configurations.begin(); \
	i != _server_configurations.end(); i++)
        if ((*i).getName() == name)
            return (*i);
    throw InvalidGetCall("There are no Servers with a matching name to " + name);
    return (_server_configurations[0]);
}

std::map<std::string, t_host_port> Config::getAllPairs() const
{
    std::map<std::string, t_host_port> servers;
    
    for (std::vector<ServerConfig>::const_iterator it = _server_configurations.begin(); \
	it != _server_configurations.end(); it++)
        servers.insert(std::make_pair((*it).getName(), (*it).getHostPortPair()));
    return (servers);
}

Config::Config(std::string path_to_config) : _server_configurations(initServerConfigs(path_to_config))
{
    if (TESTING)
        std::cout << "Config completed without any issues" << std::endl;
}

Config::~Config()
{
    
}
