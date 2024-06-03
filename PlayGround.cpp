#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <sys/wait.h>

static std::vector<std::string> split(std::string string, char delim)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(string);
    
    while (std::getline(stream, token, delim))
        tokens.push_back(token);
    tokens.erase(std::remove(tokens.begin(), tokens.end(), ""), tokens.end());
    return tokens;
}

static int	checkFileExtension( std::string const & file )
{
	std::string	extension;

	if (file.length() >= 5 && !file.compare(file.length() - 5, 3, ".php"))
		return 1;
	else if (file.length() >= 4 && !file.compare(file.length() - 5, 3, ".py"))
		return 2;
	return 0;
}

static std::vector<std::string>	extractEnvPath( char ** envp )
{
	std::vector<std::string>	env_paths;

	int i = -1;

	while (envp[++i])
	{
		std::string tmp_env_var = static_cast<std::string>(envp[i]);
		if (!tmp_env_var.compare(0, 5, "PATH="))
		{
			tmp_env_var.erase(0, 5);
			env_paths = split(tmp_env_var, ':');
			return env_paths;
		}
	}
	return env_paths;
}

static std::string	retrievePhpCgiPath( char ** envp )
{
	std::vector<std::string>	env_paths = extractEnvPath(envp);

	if (env_paths.size() == 0)
		return "";

	std::vector<std::string>::iterator it;
	for (it = env_paths.begin(); it != env_paths.end(); it++)
	{
		std::string	tmp = (*it).append("/php-cgi");
		if (access(tmp.c_str(), X_OK) == 0)
			return (tmp.append("/"));
	}
	return "";
}

static void	readfile(int read_fd, std::string & response_file)
{
	char	buffer[4096];
	ssize_t	bytes;

	response_file.clear();
	while ((bytes = read(read_fd, buffer, sizeof(buffer))) > 0)
		response_file.append(buffer, bytes);
}

static bool	executeScript( std::string & file, std::string & result, int extension, char **envp)
{
	int		pfd[2];
	pid_t	pid;
	int		status;

	if (extension == 1) //php
	{
		std::string tmp_file = retrievePhpCgiPath(envp);
		tmp_file.append(file);
		file = tmp_file;
	}

	if (pipe(pfd) == -1)
	{
		std::cerr << "Error: pipe fn issue" << std::endl;
		return false;
	}
	pid = fork();
	if (!pid)
	{
		if (dup2(pfd[1], STDOUT_FILENO) == -1 || close(pfd[0]) == -1 \
		|| close(pfd[1]) == -1)
		{
			std::cerr << "Error: dup2 or close fn issue" << std::endl;
			return false;
		}
		char *args[] = {const_cast<char *>(file.c_str()), NULL};
		execve(file.c_str(), args, envp);
		std::cerr << "Error: cannot execute " << file << std::endl;
		return false;
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status))
	{
		std::cerr << "Error: waitpid fn failed" << std::endl;
		return false;
	}
	readfile(pfd[0], result);
	if (close(pfd[1]) == -1 || close(pfd[0]) == -1)
	{
		std::cerr << "Error: dup2 or close fn issue" << std::endl;
		return false;
	}
	return true;
}

int	main(int argc, char ** argv, char **envp)
{
	std::string	file;
	std::string result;
	int	extension;

	if (argc != 2)
	{
		std::cerr << "Wrong number of elements"  << std::endl;
		return 1;
	}
	file = static_cast<std::string>(argv[1]);
	extension = checkFileExtension(file);
	if (!extension)
	{
		std::cerr << "Error: script type incorrect"  << std::endl;
		return 1;
	}
	else
	{
		if (!executeScript(file, result, extension, envp))
			return 1;
		std::cout << result;
	}
	return 0;
}
