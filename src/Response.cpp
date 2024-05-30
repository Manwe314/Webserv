/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 23:27:16 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/30 09:37:31 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
	400 - Bad request
	501 - Not Implemented
	505 - HTTP version not supported
*/

Response::Response(std::string request, ServerConfig config)
{
    int i;
    if (request.empty())
        _status_code = 400;
    _server_config = config;
    /*
		Here we need to split the request into 3 parts: request line, headers and body
    	and parse them individually according to the rules.
	*/
    i = request.find("\r\n");
    if (i == std::string::npos)
        _status_code = 400;
    else
        parseRequestLine(request.substr(0, i));
}

/*
	This function parses the request line of the message
	by convection space, new line and carrage return
	It should never be present in the request URI, method or http version.
	Therefore, we can split the string on these chars.
	The result from a valid request has 3 elements: method, URI and HTTP version.
*/
void Response::parseRequestLine(std::string request_line)
{
    std::vector<std::string> split_request_line = split(request_line, " \n\r");

	/*
		If the number of elements is not 3, the request is invalid.
	*/
    if (split_request_line.size() != 3)
    {
        _status_code = 400;
        return ;
    }
	/*
		If the first part of the string is not an http method, the request is invalid.
	*/
	if (!isValidHttpMethod(split_request_line[0]) && !isInvalidHttpMethod(split_request_line[0]))
    {
        _status_code = 400;
        return ;
    }
	/*
		If the first element is an http method but it isn't supported,
		the request is invalid with 501 (not 400).
	*/
    if (isInvalidHttpMethod(split_request_line[0]))
    {
        _status_code = 501;
        return ;
    }
	/*
		If the version is not one of valid (supported) or invalid (not supported),
		the request is invalid.
	*/
    if (!isValidVersion(split_request_line[2]) && !isInvalidVersion(split_request_line[2]))
    {
        _status_code = 400;
        return ;
    }
	/*
		If the request is one of invalid (unsupported) versions,
		the request is invalid with 505 (not 400).
	*/
    if(isInvalidVersion(split_request_line[2]))
    {
        _status_code = 505;
        return ;
    }
    _method = split_request_line[0];
    _request_URI = split_request_line[1];
    _http_version = split_request_line[2];
}

Response::~Response()
{
    
}
