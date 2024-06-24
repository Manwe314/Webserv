/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 15:02:23 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/19 15:27:04 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"
#include "Config.hpp"

class Server
{
private:
	/*
    	Requests are saved as a map
		because we already have a unique identifier for every client, the file descriptor.
		Also, all the data sent to us is a string thus a map<int, string>
	*/
    std::map<int, std::string> _requests;
    //So far, I'm assuming that all responses are strings. This probably is not true.
    std::map<int, std::string> _responses;
    //Socket address struct populated during constructor call.
    struct sockaddr_in _server_address;
    //this servers configuration.
    ServerConfig _config;    
    /*
		Server socket is already an unique identifier
		but name is just for logging purposes, to make it more readable.
	*/
    std::string _name;
    //Ipv4:Port pair.
    t_host_port _pair;
    //Server socket fd
    int _fd;

    void setAddress();
public:
    Server();
    //sets the hjost port pair and name, fd is extra, its just another way to make sure the setup() went smoothly since we call this constuctor with -1 for fd.
    //constructor calls setAddress() to populate _server_address.
    Server(t_host_port pair,std::string name ,int fd, ServerConfig config);
    ~Server();

    //Sets up the server, it is non-blocking.
    void setup();
    //Accepts connections, it is blocking.
    int accept();
    //Receives client data, it is blocking.
    void receive(int client_fd);
    //Process is like this because I assume that responses are all strings. It is non-blocking.
    void process(int client_fd);
    //Sends the processed client data, it is blocking.
    void send(int client_fd);
    //Closes the fd and deletes the coresponding request, it is non-blocking.
    void close(int client_fd);

    //Checks if a response for a given client fd is ready, it is non-blocking
    bool hasResponse(int client_fd);
    
    int getFD() const;
    t_host_port getPair() const;
    std::string getName() const;

};

std::ostream& operator<<(std::ostream& obj, Server const &server);

class SocketCreationError : std::exception
{
    private:
        std::string msg;
    public:
        SocketCreationError(const std::string& msg) : msg(msg) {}
        virtual const char * what() const throw();
        virtual ~SocketCreationError() throw() {}
    
};

class SocketBindingError : std::exception
{
    private:
        std::string msg;
    public:
        SocketBindingError(const std::string& msg) : msg(msg) {}
        virtual const char * what() const throw();
        virtual ~SocketBindingError() throw() {}
};

class ListeningError : std::exception
{
    private:
        std::string msg;
    public:
        ListeningError(const std::string& msg) : msg(msg) {}
        virtual const char * what() const throw();
        virtual ~ListeningError() throw() {}
    
};

class ClientConnectionError : std::exception
{
    private:
        std::string msg;
    public:
        ClientConnectionError(const std::string& msg) : msg(msg) {}
        virtual const char * what() const throw();
        virtual ~ClientConnectionError() throw () {}
    
};

#endif
