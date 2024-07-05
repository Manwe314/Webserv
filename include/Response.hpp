/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 23:20:55 by lkukhale          #+#    #+#             */
/*   Updated: 2024/07/05 14:09:50 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Webserv.hpp"
#include "Config.hpp"
#include "Cgi.hpp"

/*
	This class will do all the processing for a given request
	and provide a response to the server. 
	A bunch of things will be needed in this class.
*/

class Cgi;
class Response
{
private:
    std::map<std::string, std::string> _headers;
    ServerConfig _server_config;
    std::string _http_version;
    std::string _request_URI;
    std::string _method;
    std::string _body;
    std::string _path;
	int			_status_code;
	size_t		_position;
	size_t		_chunk_size;
	std::string	_query;
    t_host_port _pair;
	char		**_envp;

    void	parseRequestLine(std::string request_line);
    void	parseMessageHeaders(std::string message_headers);
    void	parseMessageBody(std::string message_body);

    std::string processGET();
    std::string processDELETE();
    std::string processPUT();
	std::string processPOST();
    std::string statusLineProcess();
    std::string handleErrorResponse();
    std::string headersProcess(std::string body, std::string path, bool is_cgi, std::string redir);
    std::string serviceGetResource(ServerRoutesConfig config, std::string uri);
	void	validCgiContentHeader(std::string response);
public:
    Response(std::string request, ServerConfig config, t_host_port pair, char **envp);
    ~Response();
    
    std::string process();
    std::string default404();
    std::string default3XX(std::string redir);
    ServerRoutesConfig matchSubRoute(std::string uri);
    std::string makeFullPath(ServerRoutesConfig config, std::string uri);

    std::map<std::string, std::string> getHeaders() const;
    ServerConfig getServerConfig() const;
    std::string getHttpVersion() const;
    std::string getURI() const;
    std::string getMethod() const;
    std::string getBody() const;
	std::string getQuery() const;
    int getStatusCode() const;
};

class NoMatchFound : public std::exception
{
    private:
        std::string msg;
    public:
        NoMatchFound(const std::string& msg) : msg(msg) {}
        virtual const char* what() const throw();
        virtual ~NoMatchFound() throw() {}
};

class TransferEncodingError : public std::exception
{
    private:
        std::string msg;
    public:
        TransferEncodingError(const std::string& msg) : msg(msg) {}
        virtual const char* what() const throw();
        virtual ~TransferEncodingError() throw() {}
};

std::ostream& operator<<(std::ostream& obj, Response const &response);

#endif
