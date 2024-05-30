/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 09:46:55 by bleclerc          #+#    #+#             */
/*   Updated: 2024/05/30 17:17:43 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Cgi.hpp"

Cgi::Cgi()
{
	return ;
}

Cgi::~Cgi()
{
	return ;
}

bool	Cgi::addRelativePath( void )
{
	std::string	extension;

	extension = _script.erase(0, _script.length() - 5);

	if (_script)
}

bool	Cgi::executeScript( void )
{
	
}