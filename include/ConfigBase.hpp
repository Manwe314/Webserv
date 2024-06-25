/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBase.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/17 19:00:39 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/24 18:28:19 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGBASE_HPP
#define CONFIGBASE_HPP

#include "Webserv.hpp"

/*
	This class contains all the possible declarations that can be found
	in the config file along with all the possible http methods that can be allowed.
	also this class houses all the implemented headers and the list of all possiable
	reason-phrases for the possible status codes.
	
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
	static const std::map<int, std::string> _reason_phrases;
	static char ** _envp;

    ConfigBase();
    ~ConfigBase();
};

#endif