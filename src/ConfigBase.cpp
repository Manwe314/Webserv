/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBase.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/17 19:02:50 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/12 18:23:35 by lkukhale         ###   ########.fr       */
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
    
    return rules;
}


static std::vector<std::string> init_http_methods() 
{
    std::vector<std::string> http_methods;

    http_methods.push_back("DELETE");
    http_methods.push_back("GET");
    http_methods.push_back("POST");
    
    return http_methods;
}

ConfigBase::ConfigBase()
{

}

ConfigBase::~ConfigBase()
{
    
}

const std::vector<std::string> ConfigBase::_rules = init_rules();
const std::vector<std::string> ConfigBase::_http_methods = init_http_methods();
