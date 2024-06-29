/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBase.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brettleclerc <brettleclerc@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/17 19:02:50 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/25 11:19:04 by brettlecler      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBase.hpp"

static std::vector<std::string> init_rules() 
{
    std::vector<std::string> rules;
    
    rules.push_back("location");
    rules.push_back("allow_methods");
    rules.push_back("root");
    rules.push_back("index");
    rules.push_back("error_page");
    rules.push_back("alias");
    rules.push_back("autoindex");
    rules.push_back("client_max_body_size");
    
    return (rules);
}


static std::vector<std::string> init_http_methods() 
{
    std::vector<std::string> http_methods;

    http_methods.push_back("DELETE");
    http_methods.push_back("GET");
    http_methods.push_back("POST");
    http_methods.push_back("PUT");
    
    return (http_methods);
}

static std::vector<std::string> init_valid_headers()
{
    std::vector<std::string> valid_headers;

    valid_headers.push_back("user-agent");
    valid_headers.push_back("host");
    valid_headers.push_back("accept");
    valid_headers.push_back("accept-language");
    valid_headers.push_back("accept-encoding");
    valid_headers.push_back("connection");
    valid_headers.push_back("cache-control");
	valid_headers.push_back("content-length");
	valid_headers.push_back("transfer-encoding");
    valid_headers.push_back("content-encoding");
    valid_headers.push_back("content-md5");
    valid_headers.push_back("content-range");
    valid_headers.push_back("content-length");
    valid_headers.push_back("content-language");

    return (valid_headers);
}

static std::map<int, std::string> init_reason_phrases()
{
    std::map<int, std::string> reason_phrases;

    reason_phrases.insert(std::make_pair(200, "OK"));
    reason_phrases.insert(std::make_pair(201, "Created"));
    reason_phrases.insert(std::make_pair(204, "No Content"));
    reason_phrases.insert(std::make_pair(400, "Bad Request"));
    reason_phrases.insert(std::make_pair(403, "Forbidden"));
    reason_phrases.insert(std::make_pair(404, "Not Found"));
    reason_phrases.insert(std::make_pair(405, "Method Not Allowed"));
    reason_phrases.insert(std::make_pair(413, "Payload Too Large"));
    reason_phrases.insert(std::make_pair(409, "Conflict"));
    reason_phrases.insert(std::make_pair(500, "Internal Server Error"));
    reason_phrases.insert(std::make_pair(501, "Not Implemented"));
    reason_phrases.insert(std::make_pair(505, "HTTP Version Not Supported"));

    return (reason_phrases);
}

ConfigBase::ConfigBase()
{

}

ConfigBase::~ConfigBase()
{
    
}

const std::vector<std::string> ConfigBase::_rules = init_rules();
const std::vector<std::string> ConfigBase::_http_methods = init_http_methods();
const std::vector<std::string> ConfigBase::_valid_headers = init_valid_headers();
const std::map<int, std::string> ConfigBase::_reason_phrases = init_reason_phrases();
