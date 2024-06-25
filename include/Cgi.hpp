/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 09:47:34 by bleclerc          #+#    #+#             */
/*   Updated: 2024/06/24 15:49:28 by lkukhale         ###   ########.fr       */
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
	std::string	retrievePhpCgiInterpreter( char ** envp );
	std::vector<std::string>	extractEnvPath( char ** envp );
	bool	readfile(int read_fd);
	void	executeScript( std::string const & file, char **envp);
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

	int	_extension;
	std::string const &	_file;
	std::string _result;
};
