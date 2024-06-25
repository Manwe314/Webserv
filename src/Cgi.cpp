/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brettleclerc <brettleclerc@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 09:46:55 by bleclerc          #+#    #+#             */
/*   Updated: 2024/06/25 12:12:17 by brettlecler      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Cgi.hpp"

Cgi::Cgi( std::string & file, char **envp ) : _file(file), _envp(envp)
{
	//	Checks if file is accessible and executable
	if (access(_file.c_str(), X_OK) != 0)
		throw CgiExecutionError("Error: " + _file + " not accessible or executable");

	_extension = determineExtension(file);
	if (!_extension)
		throw CgiExecutionError("Error: " + _file + " unsupported script format");
	else
		executeScript(_file);
}

/*
	Check the file extension and determine the type of script.
	Return 0 if script type not found.
	For now, we ceck for either PHP or Python scripts but more can be added.
*/
int	Cgi::determineExtension( std::string const & file ) const
{
	std::string type;

	type = file.substr(file.rfind(".") + 1);
	if (type == ".php")
		return 1; // PHP script
	else if (type == ".py")
		return 2; // Python script
	return 0; // Unsupported script type
}

/*
	Extract the PATH environment variable and split it into individual paths.
	First step towards retrieving the php-cgi interpreter in a system
*/
std::vector<std::string>	Cgi::extractEnvPath()
{
	std::vector<std::string>	env_paths;

	int i = -1;

	while (_envp[++i])
	{
		std::string tmp_env_var = static_cast<std::string>(_envp[i]);
		if (!tmp_env_var.compare(0, 5, "PATH="))
		{
			tmp_env_var.erase(0, 5);
			env_paths = split(tmp_env_var, ':');
			return env_paths;
		}
	}
	return env_paths;
}

/*
	Here we add the correct relative path of the PHP-CGI interpreter,
	paths extracted from the system's environment
*/
std::string	Cgi::retrievePhpCgiInterpreter()
{
	std::vector<std::string>	env_paths = extractEnvPath();

	if (env_paths.size() == 0)
		return "";

	std::vector<std::string>::iterator it;
	for (it = env_paths.begin(); it != env_paths.end(); it++)
	{
		std::string	tmp = (*it).append("/php-cgi");
		if (access(tmp.c_str(), X_OK) == 0)
			return (tmp);
	}
	return "";
}

// Function to read data from a file descriptor into a string
bool	Cgi::readfile(int read_fd)
{
	char	buffer[4096];
	ssize_t	bytes;

	_result.clear(); //Just to be sure
	while ((bytes = read(read_fd, buffer, sizeof(buffer))) > 0)
		_result.append(buffer, bytes);
	if (bytes == -1)
		return false;
	return true;
}

// Function to execute a script and capture its output
void	Cgi::executeScript( std::string const & file)
{
	int			pfd[2], status;
	pid_t		pid;
	std::string	interpreter;

	// Determine the interpreter based on the script type
	if (_extension == 1) // PHP script
	{
		interpreter = retrievePhpCgiInterpreter();
		//std::cout << interpreter << std::endl;
		if (interpreter.empty())
			throw CgiExecutionError("Error: php-cgi not found");
	}
	if (pipe(pfd) == -1)
		throw CgiExecutionError("Error: pipe fn issue");
	pid = fork();
	//Enter child process
	if (!pid)
	{
		if (dup2(pfd[1], STDOUT_FILENO) == -1 || close(pfd[0]) == -1 \
		|| close(pfd[1]) == -1)
			throw CgiExecutionError("Error: dup2 or close fn issue");
		if (_extension == 1) // PHP script
		{
			char *args[] = {const_cast<char *>(interpreter.c_str()),\
			const_cast<char *>(file.c_str()), NULL};
			execve(interpreter.c_str(), args, _envp);
		}
		else
		{
			char *args[] = {const_cast<char *>(file.c_str()), NULL};
			execve(file.c_str(), args, _envp);
		}
		close(STDOUT_FILENO); //not sure about this.
		exit(EXIT_FAILURE);
	}

	/*
		Found this solution on slack.
		Apparently helps when scripts are launched in a loop.
	*/
	while (waitpid(pid, &status, WNOHANG) > 0)
		;
	
	//Return status from child process. If not 0, error is thrown.
	if (WIFEXITED(status) && WEXITSTATUS(status))
	{
		close(pfd[0]);
		close(pfd[1]);
		throw CgiExecutionError("Error: execve: cannot execute " + _file + ": "\
		+ std::to_string(WEXITSTATUS(status)) + " exit code.");
	}
	
	/*
		Saves the execve content from the read end of the pipe
		into the string _result variable
	*/
	if (!readfile(pfd[0]))
		throw CgiExecutionError("Error: read fn failed");

	if (close(pfd[1]) == -1 || close(pfd[0]) == -1)
		throw CgiExecutionError("Error: dup2 or close fn issue");
}


/**** UNUSED FUNCTIONS ****/

Cgi::Cgi()
{
	return ;
}

Cgi::~Cgi()
{
	return ;
}

Cgi::Cgi( Cgi const & src ) {
	*this = src;
	return ;
}

Cgi &	Cgi::operator=( Cgi const & rhs ) {
	(void)rhs;
	return *this;
}


std::string	Cgi::getCgiResult( void ) const {
	return _result;
}

const char * Cgi::CgiExecutionError::what() const throw()
{
    return (msg.c_str());
}
