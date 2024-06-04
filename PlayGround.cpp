#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <sys/wait.h>
#include <unistd.h>

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

// Function to check the file extension and determine the type of script (PHP or Python)
static int	checkFileExtension( std::string const & file )
{
	std::string	extension;

	if (file.length() >= 5 && !file.compare(file.length() - 4, 4, ".php"))
		return 1; // PHP script
	else if (file.length() >= 4 && !file.compare(file.length() - 3, 3, ".py"))
		return 2; // Python script
	return 0; // Unsupported script type
}

// Function to extract the PATH environment variable and split it into individual paths
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

// Function to retrieve the path of the PHP CGI interpreter
static std::string	retrievePhpCgiInterpreter( char ** envp )
{
	std::vector<std::string>	env_paths = extractEnvPath(envp);

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
static void	readfile(int read_fd, std::string & response_file)
{
	char	buffer[4096];
	ssize_t	bytes;

	response_file.clear();
	while ((bytes = read(read_fd, buffer, sizeof(buffer))) > 0)
		response_file.append(buffer, bytes);
	if (bytes == -1)
		std::cerr << "Error: read fn failed" << std::endl;
}

// Function to execute a script and capture its output
static bool	executeScript( std::string & file, std::string & result, int extension, char **envp)
{
	int			pfd[2];
	pid_t		pid;
	int			status;
	std::string	interpreter;

	// Determine the interpreter based on the script type
	if (extension == 1) // PHP script
	{
		interpreter = retrievePhpCgiInterpreter(envp);
		if (interpreter.empty())
        {
            std::cerr << "Error: php-cgi not found" << std::endl;
            return false;
        }
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
		if (extension == 1) // PHP script
		{
			char *args[] = {const_cast<char *>(interpreter.c_str()),\
			const_cast<char *>(file.c_str()), NULL};
			execve(interpreter.c_str(), args, envp);
		}
		else
		{
			char *args[] = {const_cast<char *>(file.c_str()), NULL};
			execve(file.c_str(), args, envp);
		}
		std::cerr << "Error: cannot execute " << file << std::endl;
		exit(EXIT_FAILURE);
	}
	if (close(pfd[1]) == -1)
	{
		std::cerr << "Error: dup2 or close fn issue" << std::endl;
		return false;
	}
	while (waitpid(pid, &status, WNOHANG) > 0)
		;
	if (WIFEXITED(status) && WEXITSTATUS(status))
	{
		std::cerr << "Error: waitpid fn failed" << std::endl;
		return false;
	}
	readfile(pfd[0], result);
	if (close(pfd[0]) == -1)
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
