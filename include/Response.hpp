/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 23:20:55 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/12 16:43:09 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Webserv.hpp"
#include "Config.hpp"

//this class will do all the processing for a given request and provide a response to the server. 
//a bunch of things will be needed inm this class.
class Response
{
private:
    ServerConfig _server_config;
    int _status_code;
    std::string _method;
    std::string _request_URI;
    std::string _http_version;

    void parseRequestLine(std::string request_line);
public:
    Response(std::string request, ServerConfig config);
    ~Response();
};



#endif
