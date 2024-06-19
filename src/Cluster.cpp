/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 01:14:37 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/19 14:53:47 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"
#include "Webserv.hpp"


static std::vector<ServerConfig> serveAlternatives(std::vector<ServerConfig> servers, t_host_port pair, std::string name)
{
    std::vector<ServerConfig> alternatives;
    std::vector<t_host_port> pairs;

    for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        pairs = (*it).getHostPortPair();
        for (std::vector<t_host_port>::iterator i = pairs.begin(); i != pairs.end(); i++)
        {
            if ((*i) == pair && name != (*it).getName())
            {
                alternatives.push_back((*it));
            }
        }
    }
    //printVector(alternatives);
    return (alternatives);
}

//Cluster config that creates the servers and prepares them for run().
Cluster::Cluster(Config conf, char **envp) : _envp(envp)
{
	(void)_envp;
    //for Server constuctor we only need the host port pair and name, so we use a map with string and t_host_port key value pair.
    std::vector<ServerConfig> servers;
    std::vector<t_host_port> bound_pairs;
    std::vector<t_host_port> pairs;

    //config member function to return all succsessfuly configured servers name and t_host_pair.        
    servers = conf.getServerConfigs();

    //we need to itterate over every element of this map and for each:
    for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        pairs = (*it).getHostPortPair();
        for (std::vector<t_host_port>::iterator i = pairs.begin(); i != pairs.end(); i++)
        {
            if (std::find(bound_pairs.begin(), bound_pairs.end(), (*i)) == bound_pairs.end())
            {
                //call a server constuctor with t_host_port and name (fd set at -1, fd will be set by the setup() function of the server itself).
                Server server((*i), (*it).getName(), -1, (*it), serveAlternatives(servers, (*i), (*it).getName()));
                try
                {
                    //std::cout << server.getPair() << std::endl;
                    //Call setup() member function
                    server.setup();
                    //Make a pair of fd and server and insert it to _servers variable
                    _servers.insert(std::make_pair(server.getFD(), server));
                    bound_pairs.push_back((*i));
                }
                catch (const  ListeningError& e)
                {
                    std::cout << e.what() << std::endl;
                }
                catch (const SocketBindingError& e)
                {
                    std::cout << e.what() << std::endl;
                }
            }
        }
    }
    //printMap<int, Server>(_servers);
}

/*
	Initializer for pollfd struct.
	The struct is passed by reference and is populated by server fds already set up.
*/
int Cluster::initPollFds(struct pollfd (&fds)[MAX_EVENTS]) 
{
    int index = 0;
    for (std::map<int, Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        if (index > MAX_EVENTS)
            break;
        fds[index].fd = (*it).first;
        fds[index].events = POLLIN;
		//Only accept() is used on server sockets so POLLIN (data ready to read) is enough.
        index++;
    }
    //Return the number of fds.
    return (index);
}

bool Cluster::isServerSocket(int fd)
{
    std::map<int, Server>::iterator it;
    
    it = _servers.find(fd);
    if (it != _servers.end())
        return (true);
    return (false);
}

/*
	Uses the client fd [key],
	server fd[value] pair map to check if a given fd is a client fd or a server fd.
*/
static bool isClientSocket(std::map<int, int> map, int fd)
{
    std::map<int, int>::iterator it = map.find(fd);
    if (it != map.end())
        return (true);
    return (false);
}

//Map erasing function using an int key.
static void mapErase(std::map<int, int> &map, int key)
{
    std::map<int, int>::iterator it = map.find(key);
    if (it != map.end())
        map.erase(it);
}

void Cluster::run()
{
    struct pollfd fds[MAX_EVENTS];
	/*
		Map to save server ownership.
		This way, we will always know which client fds are serviced by which servers. 
		First int [key] is client fd (since each client is unique fd for ALL the servers). 
		Second int [value] is server fd (since each server may service more than 1 client).
	*/
    std::map<int, int> client_server;
    //Dots to make "waiting for connection" message animate :)
    std::string wait_messages[4] = {
		"Waiting for connection   ",
		"Waiting for connection.  ",
		"Waiting for connection.. ",
		"Waiting for connection..."};
    //Poll timeout set by TIMEOUT_SEC defined.
    int timeout_ms = TIMEOUT_SEC * 500;
    int index = 0;
    int client_fd;
    int nfds;
    int ret;

    //Get the number of fds for poll by initializing the struct with server socket fds.
    nfds = initPollFds(fds);
    
    //The runtime loop where all the cluster work happens.
    while (true)
    {
        /*
			Poll checks the fds in its struct and returns the number fds
			that are ready to perfor an operation
			(read or write, or in our case, recv, send or accept).
		*/
        ret = poll(fds, nfds, timeout_ms);

        if (ret == -1) //If ret is negative, poll failed so we quit.
        {
            std::cout << "POLL FAILED" << std::endl;
            return ;
        }
		/*
			If ret is 0 the timeout time has passed,
			which means that we have nothing happening
			and that we are simply waiting for client connections.
		*/
        else if (ret == 0)
        {
            std::cout << "\r" << wait_messages[index] << std::flush;
			index = (index + 1) % 4;
        }
        else //In any other case at least 1 fd is ready to perform some action.
        {
			/*
            	Loop over each pollfd array member.
				In its struct, the revents variable is set to the appropriate action
				on the appropriate fd.
			*/
            for (int i = 0; i < nfds; i++)
            {
				/*
					If the revents varibale is set to POLLIN,
					that means that there is data to read,
					thus either an accept() or recv() is available to perform.
				*/
                if (fds[i].revents & POLLIN)
                {
					/*
                    	To distingush between accept() and recv(),
						we can just check if the fd (the fd whose revents is set to POLLIN)
						is a server socket fd or not.
					*/
					/*
						If the fd is a server fd accept() is ready to perform.
					*/
                    if (isServerSocket(fds[i].fd))
                    {
                        try
                        {
                            //Try to accept on the server identified by its [key] A.K.A fd.
                            client_fd = _servers[fds[i].fd].accept();
							/*
                            	If accept fails an exception will be thrown
								and everything below wont happen.
							*/
							/*
                            	If accept does not fail,
								add the new client to the client_server map
								and add that fd in the pollfd struct to track now.
							*/
                            client_server.insert(std::make_pair(client_fd, fds[i].fd));
                            fds[nfds].fd = client_fd;
                            fds[nfds].events = POLLIN | POLLOUT;
                            nfds++;
                        }
                        catch(const ClientConnectionError& e)
                        {
                            std::cerr << e.what() << std::endl;
                        }
						/*
                        	This break is paramount,
							without it an timing issue comes up
							where accept() makes the newly made client fd ready for read()
							even when there is no data to read().
						*/
                        break;
                    }
					/*
						If it isn't a server socket, then it is a client socket.
						For that we read the data sent to us and process it.
					*/
                    else
                    {
                        try
                        {
							/*
                            	Try to receive data.
								If it fails, receive will throw an error
								and the process wont happen.
							*/
                            _servers[client_server[fds[i].fd]].receive(fds[i].fd);
                            _servers[client_server[fds[i].fd]].process(fds[i].fd);
                        }
                        catch(const ClientConnectionError& e)
                        {
							/*
                            	If receive throws an error, it will close the client fd.
								We need to erase it from our client_server map
								and from pollfds struct.
							*/
                            mapErase(client_server, fds[i].fd);
                            for (int j = i; j < nfds - 1; j++)
                                fds[j] = fds[j + 1];
                            nfds--;
                            std::cerr << e.what() << std::endl;
                        }
                    }
                    
                }
				/*
					If the revent is POLLOUT,
					specifically on a client fd and not STDOUT,
					that means that we are ready to send() data.
				*/
                else if ((fds[i].revents & POLLOUT) && isClientSocket(client_server, fds[i].fd))
                {
                    try
                    {
                        //Try to send data.
                        _servers[client_server[fds[i].fd]].send(fds[i].fd);
                    }
                    catch(const ClientConnectionError& e)
                    {
						/*
                        	If send fails, it will close the client fd.
							We need to erase it from our client_Server map
							and from pollfds struct.
						*/
                        mapErase(client_server, fds[i].fd);
                        for (int j = i; j < nfds - 1; j++)
                            fds[j] = fds[j + 1];
                        nfds--;
                        std::cerr << e.what() << std::endl;
                    }
                }
            }
        }
    }
}

Cluster::~Cluster() 
{
    
}
