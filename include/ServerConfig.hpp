/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 20:35:33 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/06 17:39:07 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Webserv.hpp"
#include "ServerRoutesConfig.hpp"

//This class hold all the configuration for a single server.
class ServerConfig
{
private:
    //host:port pair where host is stored as uint and port as int.
    t_host_port _pair;
    std::string _server_name;
    //Global settings for this server outside any "location" block but inside the "server" block
    ServerRoutesConfig _serverwide_config;
    //A list of all settings for each route/location block in the config file
    std::vector<ServerRoutesConfig> _route_configs;

    t_host_port initHostPort(std::vector<std::string> server_block);
    std::string initServerName(std::vector<std::string> server_block);
    ServerRoutesConfig initServerWideConfig(std::vector<std::string> server_block);
    std::vector<ServerRoutesConfig> initRouteConfigs(std::vector<std::string> server_block);
    
public:
    ServerConfig();
    ServerConfig(std::vector<std::string> server_block);
    ~ServerConfig();
    
    ServerConfig& operator=(const ServerConfig& rhs);

    t_host_port getHostPortPair() const;
    std::string getName() const;
    ServerRoutesConfig getServerWideConfig() const;
    std::vector<ServerRoutesConfig> getRouteConfigs() const;
    ServerRoutesConfig getRouteConfigByLocation(std::string location) const;
};

class HostDeclarationIssue : std::exception
{
    public:
        virtual const char * what() const throw();
};

class BracketPairMissMatch : std::exception
{
    private:
        std::string msg;
    public:
        BracketPairMissMatch(const std::string& msg) : msg(msg) {}
        virtual const char * what() const throw();
        virtual ~BracketPairMissMatch() throw () {}
};

#endif
