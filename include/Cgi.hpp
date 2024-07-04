/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 09:47:34 by bleclerc          #+#    #+#             */
/*   Updated: 2024/07/04 16:30:11 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

class Cgi
{
	public:
	Cgi( std::string & file, char **envp, std::string const & body );
	virtual ~Cgi();
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

	int	determineExtension( std::string const & file ) const;
	std::string	retrievePhpCgiInterpreter();
	std::vector<std::string>	extractEnvPath();
	bool	readfile(int read_fd);
	void	ft_execve( std::string const & file, std::string const & body );
	void	executeScript( std::string const & file, std::string const & body );

	int			_extension;
	std::string	_file;
	std::string _result;
	char		**_envp;
	bool		is_post;
	std::string	_interpreter;
};
