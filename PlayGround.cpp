#include <string>
#include <iostream>
#include <fstream>

static int	checkFileExtension( std::string const & file )
{
	std::string tmp_file = file;
	std::string	extension;

	if (file.length() > 4)
	{
		extension = tmp_file.erase(0, tmp_file.length() - 4);
		if (extension == ".php");
		{
			
			return 1;
		}
	}
	else if (file.length() > 3)
	{
		extension = tmp_file.erase(0, tmp_file.length() - 3);
		if (extension == ".py")
			return 2;
	}
	return 0;
}

void	addRelativePath( std::string & file )
{
	//fastCGI search through envp;
}

static bool	executeScript( std::string & file, int extension, char **envp)
{
	int	fd[2];
	
	if (extension == 1) //php
		addRelativePath(file);

	if (pipe(fd) == -1)
		return false;
	//fork

	//child process execve(file, NULL, envp);

	//waitpid(WNOHANG)

	//read parent processor and stock in response.

	//close all crap	
	
	return true
}

int	main(int argc, char ** argv, char **envp)
{
	std::string	file;
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
		if (!executeScript(file, extension, envp))
			return 1;
	}
	return 0;
}
