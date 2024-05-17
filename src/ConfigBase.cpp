/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBase.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/17 19:02:50 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/16 21:35:59 by lkukhale         ###   ########.fr       */
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
    
    return (rules);
}


static std::vector<std::string> init_http_methods() 
{
    std::vector<std::string> http_methods;

    http_methods.push_back("DELETE");
    http_methods.push_back("GET");
    http_methods.push_back("POST");
    
    return (http_methods);
}

static std::vector<std::string> init_valid_headers()
{
    std::vector<std::string> valid_headers;

    valid_headers.push_back("user-agent");
    valid_headers.push_back("accept");
    valid_headers.push_back("accept-language");
    valid_headers.push_back("accept-encoding");
    valid_headers.push_back("connection");
    valid_headers.push_back("cache-control");

    return (valid_headers);
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
