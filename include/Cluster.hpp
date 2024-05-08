/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 16:59:06 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/07 22:49:17 by lkukhale         ###   ########.fr       */
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
    //the main map for all the servers. the key int is the server fd, since its already a unique identifier of all servers. value Server is the object of class Server.
    std::map<int, Server> _servers;
    
    //initializer for poll struct. populates it with server fds .
    int initPollFds(struct pollfd (&fds)[MAX_EVENTS]);
    //returns true if a given fd is a server socket and false if its not.
    bool isServerSocket(int fd);
public:
    //constructor that takes in config and sets up all the servers in _servers map.
    Cluster(Config conf);
    ~Cluster();

    //main function of the cluster that runs everything.
    void run();
};


#endif
