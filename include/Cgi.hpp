/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brettleclerc <brettleclerc@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 09:47:34 by bleclerc          #+#    #+#             */
/*   Updated: 2024/06/25 12:10:53 by brettlecler      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

class Cgi
{
	public:
	Cgi( std::string & file, char **envp );
	virtual ~Cgi();
	 
	int	determineExtension( std::string const & file ) const;
	std::string	retrievePhpCgiInterpreter();
	std::vector<std::string>	extractEnvPath();
	bool	readfile(int read_fd);
	void	executeScript( std::string const & file);
	std::string	getCgiResult( void ) const;

	class CgiExecutionError : public std::exception
	{
    private:
        std::string msg;
    public:
        CgiExecutionError(const std::string& msg) : msg(msg) {}
        virtual const char * what() const throw();
        virtual ~CgiExecutionError() throw() {}
	};

	private:	
	Cgi();
	Cgi( Cgi const & src );
	Cgi &	operator=( Cgi const & rhs );

	int			_extension;
	std::string	_file;
	std::string _result;
	char		**_envp;
};
