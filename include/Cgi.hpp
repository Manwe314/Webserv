/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 09:47:34 by bleclerc          #+#    #+#             */
/*   Updated: 2024/05/30 17:11:32 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

class Cgi
{
	public:
	Cgi();
	virtual ~Cgi();
	bool	addRelativePath( void );
	bool	executeScript( void );

	private:	
	Cgi( Cgi const & src );
	Cgi &	operator=( Cgi const & rhs );
	
	std::string	_script;
};
