/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRoutesConfig.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 17:09:31 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/21 16:02:04 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERROUTESCONFIG_HPP
#define SERVERROUTESCONFIG_HPP

#include "Webserv.hpp"
#include "ConfigBase.hpp"

/*
    a to-do list when adding a rule:
        1. add the rule to the ConfigBase classes rules vector. MAKE SURE to not mess up the order (add it to the end).
        2. add the variable type(s) that will hold the information from this rule.
            2.1. make sure to add any containers in the initializer list (IN ORDER) so they initialize to "empty"
        3. in the CPP file (again IN ORDER of ConfigBase) add the rule to the enum.
        4. in the Switch case of the readRule function add the newly added case and all apropriate functionality.
        5. add a getter for all newly added member variables for this new rule.
        6. in the "<<" operator overload function add the necasarry functionality for the new member variables
        7. in the "=" operator overload function add the necasarry functionality for the new member variables
        8. in the inherit function if this new rule adds data that must be inherited by child routes add the required functinality.
        9. in the Response class add all the functionality that is relevant to this new rule.
*/

class ServerRoutesConfig;

typedef struct s_route_match
{
    int match_length;
    ServerRoutesConfig* route;
}   t_route_match;

class ServerRoutesConfig
{
private:
    //Root path
    std::string _root;
    //Location path
    std::string _location;
    //A list of file names to be looked for and served in case of a directory url request
    std::vector<std::string> _index_files;
    //A list of all http methods that are allowed.
    std::vector<std::string> _allowed_methods;
    //a list of all the first level sub routes for this subroute.
    std::vector<ServerRoutesConfig> _sub_routes;
    //a map of error page URI's with key being status code and value the URI
    std::map<int, std::string> _error_pages;
    //alias path
    std::string _alias;
    //autoindex state (-1, not declared. 0, declared to off. 1 declared to on).
    int _autoindex;
    //max client body size in bytes.
    size_t _max_body_size;
    
    //A function that helps with initilization
    void readRule(std::vector<std::string>::iterator input, std::vector<std::string>& route_block, std::vector<std::string>::const_iterator rule);
    bool isRule(std::string input);
    
public:
    ServerRoutesConfig();
    ServerRoutesConfig(std::vector<std::string> route_block, std::string location);
    ~ServerRoutesConfig();

    void inherit(ServerRoutesConfig parent);
    
    t_route_match findRouteMatch(std::string uri, ServerRoutesConfig serverwide);
    ServerRoutesConfig* findRootRoute();
    std::string serveCustomError(int status);

    ServerRoutesConfig& operator=(const ServerRoutesConfig& rhs); 

    std::string getRoot() const;
    std::string getLocation() const;
    std::vector<std::string> getIndex() const;
    std::vector<std::string> getMethods() const;
    std::vector<ServerRoutesConfig> getSubRoutes() const;
    std::map<int, std::string> getErrorPages() const;
    std::string getAlias() const;
    int getAutoindex() const;
    size_t getMaxBodySize() const;
};
std::ostream& operator<<(std::ostream& obj, ServerRoutesConfig const &conf);

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
