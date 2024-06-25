/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brettleclerc <brettleclerc@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 19:33:28 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/25 11:17:04 by brettlecler      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Webserv.hpp"
#include "ServerConfig.hpp" 
/*
    The config class holds a list(vector) of server configs,
	they in turn hold a list(vector) of route configs.

    _________________________________________
    |                config                |
    |                                      |
    |      _____________________________   |
    |     |       server config        |   |
    |     |     ________    ________   |   |
    |     |    | route |   | route |   |   |
    |     |   |_______|   |_______|    |   |
    |     |____________________________|   |
    |      _____________________________   |
    |     |       server config        |   |
    |     |     ________    ________   |   |
    |     |    | route |   | route |   |   |
    |     |   |_______|   |_______|    |   |
    |     |____________________________|   |
    |      _____________________________   |
    |     |       server config        |   |
    |     |     ________    ________   |   |
    |     |    | route |   | route |   |   |
    |     |   |_______|   |_______|    |   |
    |     |____________________________|   |
    |                                      |
    |______________________________________|
*/

class Config
{
private:
    std::vector<ServerConfig> _server_configurations;

    std::vector<ServerConfig> initServerConfigs(std::string path_to_config);
public:
    Config(std::string path_to_config);
    ~Config();

    std::vector<ServerConfig> getServerConfigs() const;

    ServerConfig getServerConfigByName(std::string name) const;

};

class CouldNotOpenFile : public std::exception
{
    public:
        virtual const char * what() const throw();

};


#endif
