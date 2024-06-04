/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 23:20:55 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/04 14:21:40 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Webserv.hpp"
#include "Config.hpp"

/*
	This class will do all the processing for a given request
	and provide a response to the server. 
	A bunch of things will be needed in this class.
*/
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
