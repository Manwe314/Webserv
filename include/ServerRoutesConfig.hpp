/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRoutesConfig.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 17:09:31 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/03 15:35:43 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERROUTESCONFIG_HPP
#define SERVERROUTESCONFIG_HPP

#include "Webserv.hpp"
#include "ConfigBase.hpp"

class ServerRoutesConfig
{
private:
    //root path
    std::string _root;
    //location path
    std::string _location;
    //a list of file names to be looked for and served in case of a directory url request
    std::vector<std::string> _index_files;
    //a list of all http methods that are allowed.
    std::vector<std::string> _allowed_methods;
    //a list of all the first level sub routes for this subroute.
    std::vector<ServerRoutesConfig> _sub_routes;

    //a function that helps with initilization
    void readRule(std::vector<std::string>::iterator input, std::vector<std::string>& route_block, std::vector<std::string>::const_iterator rule);
    bool isRule(std::string input);
    
public:
    ServerRoutesConfig();
    ServerRoutesConfig(std::vector<std::string> route_block, std::string location);
    ~ServerRoutesConfig();

    ServerRoutesConfig& operator=(const ServerRoutesConfig& rhs); 

    std::string getRoot() const;
    std::string getLocation() const;
    std::vector<std::string> getIndex() const;
    std::vector<std::string> getMethods() const;
    std::vector<ServerRoutesConfig> getSubRoutes() const;
};

class InvalidGetCall : public std::exception
{
    private:
        std::string msg;
    public:
        InvalidGetCall(const std::string& msg) : msg(msg) {}
        virtual const char* what() const throw();
        virtual ~InvalidGetCall() throw() {}
};

#endif
