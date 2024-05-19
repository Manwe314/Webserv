/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 15:02:23 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/19 19:16:09 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"
#include "Config.hpp"


class Server
{
private:
    //requests are saved as a map because we already have a unique identifier for every client, the file descriptor. also all the data sent to us is a string thus a map<int, string>
    std::map<int, std::string> _requests;
    //so far im assuming that all responses are strings, this probably is not true.
    std::map<int, std::string> _responses;
    //socket address struct populated during constructor call
    struct sockaddr_in _server_address;
    //this servers configuration.
    ServerConfig _config;    
    //server socket is already an unique identifier but name is just for loging, making it more readable.
    std::string _name;
    //Ipv4:Port pair.
    t_host_port _pair;
    //server socket fd
    int _fd;

    void setAddress();
public:
    Server();
    //sets the hjost port pair and name, fd is extra, its just another way to make sure the setup() went smoothly since we call this constuctor with -1 for fd.
    //constructor calls setAddress() to populate _server_address.
    Server(t_host_port pair,std::string name ,int fd, ServerConfig config);
    ~Server();

    //sets up the server, is not blocking.
    void setup();
    //accepts connections, is blocking.
    int accept();
    //receives client data, is blocking.
    void receive(int client_fd);
    //process is like this because i assume that responsses are all strings. is not blocking.
    void process(int client_fd);
    //sends the processed client data, is blocking.
    void send(int client_fd);
    //closes the fd and deletes the coresponding request, is not blocking.
    void close(int client_fd);

    //checks if a response for a given client fd is ready, is not blocking
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
