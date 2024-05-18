/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 23:20:55 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/18 21:01:31 by lkukhale         ###   ########.fr       */
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
    std::map<std::string, std::string> _headers;
    ServerConfig _server_config;
    std::string _http_version;
    std::string _request_URI;
    std::string _method;
    std::string _body;
    int _status_code;

    void parseRequestLine(std::string request_line);
    void parseMessageHeaders(std::string message_headers);
    void parseMessageBody(std::string message_body);
public:
    Response(std::string request, ServerConfig config);
    ~Response();

    std::map<std::string, std::string> getHeaders() const;
    ServerConfig getServerConfig() const;
    std::string getHttpVersion() const;
    std::string getURI() const;
    std::string getMethod() const;
    std::string getBody() const;
    int getStatusCode() const;
};

std::ostream& operator<<(std::ostream& obj, Response const &response);

#endif
