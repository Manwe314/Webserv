/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brettleclerc <brettleclerc@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 21:53:46 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/25 11:18:42 by brettlecler      ###   ########.fr       */
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
	
	//rm comment fn
	entire_file = removeComments(entire_file);
    
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


Config::Config(std::string path_to_config) : _server_configurations(initServerConfigs(path_to_config))
{
    if (TESTING)
        std::cout << "Config completed without any issues" << std::endl;
}

Config::~Config()
{
    
}
