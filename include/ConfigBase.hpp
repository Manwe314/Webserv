/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBase.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/17 19:00:39 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/12 18:07:02 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGBASE_HPP
#define CONFIGBASE_HPP

#include "Webserv.hpp"

//this class contains all the possible declaration that can be found in the config file along with all the possible http methods that can be allowed.
//this class is to be used for comparison to check weather a given string is a "rule" or not from the config file.
//the arctiecture of the config file will be pressumed therefore any text that should be a rule but dosent match anything in this class is discarded.
class ConfigBase
{
public:

    static const std::vector<std::string>  _rules;
    static const std::vector<std::string>  _http_methods;

    ConfigBase();
    ~ConfigBase();
};



#endif