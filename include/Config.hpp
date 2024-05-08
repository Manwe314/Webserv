/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 19:33:28 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/01 23:59:39 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Webserv.hpp"
#include "ServerConfig.hpp" 
/*
    the config class holds a list(vector) of  server configs, they in turn hold a list(vector) of route configs

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

    std::string readFile(std::string full_name);

    std::vector<ServerConfig> getServerConfigs() const;

    ServerConfig getServerConfigByName(std::string name) const;

    std::map<std::string, t_host_port> getAllPairs() const;
};

class CouldNotOpenFile : std::exception
{
    public:
        virtual const char * what() const throw();

};


#endif
