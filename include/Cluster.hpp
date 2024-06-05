/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 16:59:06 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/05 16:40:35 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Webserv.hpp"
#include "Config.hpp"
#include "Server.hpp"

class Cluster
{
private:
	/*
    	The main map for all the servers.
		The key int is the server fd,
		since its already a unique identifier of all servers.
		Value Server is the object of class Server.
	*/
    std::map<int, Server> _servers;
    
    //Initializer for poll struct. populates it with server fds .
    int initPollFds(struct pollfd (&fds)[MAX_EVENTS]);
    /*
		Returns true if a given fd is a server socket
		and false if its not.
	*/
    bool isServerSocket(int fd);
	
	//Stocks the system's environment required for php-cgi
	char **_envp;
public:
	/*
    	Constructor that takes in config
		and sets up all the servers in _servers map.
	*/
    Cluster(Config conf, char **envp);
    ~Cluster();

    //Main function of the cluster that runs everything.
    void run();
};

#endif
