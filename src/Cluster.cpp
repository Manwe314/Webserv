/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 01:14:37 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/19 19:18:47 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"
#include "Webserv.hpp"


//cluster config that makes the servers and makes them ready for run().
Cluster::Cluster(Config conf)
{
    //for Server constuctor we only need the host port pair and name, so we use a map with string and t_host_port key value pair.
    std::vector<ServerConfig> servers;

    //config member function to return all succsessfuly configured servers name and t_host_pair.        
    servers = conf.getServerConfigs();

    //we need to itterate over every element of this map and for each:
    for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        //call a server constuctor with t_host_port and name (fd set at -1, fd will be set by the setup() function of the server itself).
        Server server((*it).getHostPortPair(), (*it).getName(), -1, (*it));
        try
        {
            //call setup() member function
            server.setup();
            //make a pair of fd and server and insert it to _servers varibale
            _servers.insert(std::make_pair(server.getFD(), server));
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
    //printMap<int, Server>(_servers);
}

//initializer for pollfd struct. the struct is passed by referance and is populated by already setup server fds.
int Cluster::initPollFds(struct pollfd (&fds)[MAX_EVENTS]) 
{
    int index = 0;
    for (std::map<int, Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        if (index > MAX_EVENTS)
            break;
        fds[index].fd = (*it).first;
        fds[index].events = POLLIN; //only accept() is used on server sockets so POLLIN (data ready to read) is enough.
        index++;
    }
    //return the number of fds.
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

//uses the client fd [key], server fd[value] pair map to check if a given fd is a client fd or a server fd.
static bool isClientSocket(std::map<int, int> map, int fd)
{
    std::map<int, int>::iterator it = map.find(fd);
    if (it != map.end())
        return (true);
    return (false);
}

//map eraseing fucntion using an int key.
static void mapErase(std::map<int, int> &map, int key)
{
    std::map<int, int>::iterator it = map.find(key);
    if (it != map.end())
        map.erase(it);
}

void Cluster::run()
{
    struct pollfd fds[MAX_EVENTS];
    //map to save server ownership to always know wich client fds are serviced by which servers. 
    //first int [key] is client fd (since each client is unique fd for ALL the servers) 
    //second int [value] is server fd (since each server may service more than 1 client).
    std::map<int, int> client_server;
    //dots to make fun "waiting for connection" animation.
    std::string dots[3] = {".", "..", "..."};
    //poll timeout set by TIMEOUT_SEC define.
    int timeout_ms = TIMEOUT_SEC * 1000;
    int dot_n = -1;
    int client_fd;
    int nfds;
    int ret;

    // get the number of fds for poll by initializing the struct with server socket fds.
    nfds = initPollFds(fds);
    
    //the runtime loop where all the cluster functionality happens
    while (true)
    {
        //poll checks the fds in its struct and returns the number fds that are ready to perfor an operation (read or write, in our case recv, send or accept).
        ret = poll(fds, nfds, timeout_ms);

        
        if (ret == -1) // if ret is negative, poll failed so we quit.
        {
            std::cout << "POLL FAILED" << std::endl;
            return ;
        }
        else if (ret == 0) //if ret is 0 the timeout time has passed, that means we have nothing happening, we are waiting for client connections
        {
            if (dot_n == 2)
                dot_n = 0;
            else
                dot_n++;
            std::cout << "Waiting for connection" << dots[dot_n] << std::endl;
        }
        else //in any other case at least 1 fd is ready to perform some action.
        {
            //loop over each pollfd array member. in its struct the revents variable is set to the appropriate action on the appropriate fd.
            for (int i = 0; i < nfds; i++)
            {
                if (fds[i].revents & POLLIN) //if the revents varibale is set to POLLIN that means there is data to read, thus eaither an accept() or recv() is avalible to perform.
                {
                    //to distingush between accept() and recv() we can just check if the fd (the file descritor whose revents is set to POLLIN) is server socket fd or not
                    if (isServerSocket(fds[i].fd)) //if the fd is a server fd accept() is ready to perform.
                    {
                        try
                        {
                            //try to accept on the server identified by its [key] A.K.A fd.
                            client_fd = _servers[fds[i].fd].accept();
                            //if accept fails an exception will be thrown and everything below wont happen.
                            //if accept did not fail, add the new client to the client_server map and add that fd in the pollfd struct to track now.
                            client_server.insert(std::make_pair(client_fd, fds[i].fd));
                            fds[nfds].fd = client_fd;
                            fds[nfds].events = POLLIN | POLLOUT;
                            nfds++;
                        }
                        catch(const ClientConnectionError& e)
                        {
                            std::cerr << e.what() << std::endl;
                        }
                        //this break is paramount, without it an timing issue comes up where accept() makes the newly made client fd readty for read() even when there is no data to read().
                        break;
                    }
                    else //if it was not a server socket, then it was a client socket, for that we read the data sent to us and process it.
                    {
                        try
                        {
                            //try to recive data, if it fails recieve will throw an error and process wont happen.
                            _servers[client_server[fds[i].fd]].receive(fds[i].fd);
                            _servers[client_server[fds[i].fd]].process(fds[i].fd);
                        }
                        catch(const ClientConnectionError& e)
                        {
                            //if recive throws an error it will close the client fd, we need to erase it from our client_server map and from pollfds struct.
                            mapErase(client_server, fds[i].fd);
                            for (int j = i; j < nfds - 1; j++)
                                fds[j] = fds[j + 1];
                            nfds--;
                            std::cerr << e.what() << std::endl;
                        }
                    }
                    
                }
                else if ((fds[i].revents & POLLOUT) && isClientSocket(client_server, fds[i].fd)) //if the revent is POLLOUT, and specifically on a client fd and not STDOUT that means we are ready to send() data.
                {
                    try
                    {
                        //try to send data.
                        _servers[client_server[fds[i].fd]].send(fds[i].fd);
                    }
                    catch(const ClientConnectionError& e)
                    {
                        //if send fails it will close the client fd, so we need to erase it from our client_Server map and from pollfds struct.
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
