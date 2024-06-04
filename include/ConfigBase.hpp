/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBase.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/17 19:00:39 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/04 14:20:42 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGBASE_HPP
#define CONFIGBASE_HPP

#include "Webserv.hpp"

/*
	This class contains all the possible declarations that can be found
	in the config file along with all the possible http methods that can be allowed.
	
	This class is to be used for comparison
	to check weather a given string is a "rule" or not from the config file.
	
	The architecture of the config file will be presumed.
	Therefore, any text that should be a rule but doesn't match anything
	in this class is discarded.
*/
class ConfigBase
{
public:

    static const std::vector<std::string>  _rules;
    static const std::vector<std::string>  _http_methods;
    static const std::vector<std::string>   _valid_headers;

    ConfigBase();
    ~ConfigBase();
};

#endif