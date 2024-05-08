/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 16:12:04 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/08 21:18:33 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


/*
    for now the Server class is set up just so the i/o multiplexing works it has 0 http functionallity. (no testing done yet).
    cluster under construction.
*/
#include "Server.hpp"

//initialize the class memeber variables. including the requests map to an empty map.
Server::Server(t_host_port pair, std::string name ,int fd) : _requests(), _responses()
{
    _pair.host = pair.host;
    _pair.port = pair.port;
    _name = name;
    _fd = fd;
    //populate the server address struct
    this->setAddress();
}

Server::~Server() 
{
    
}

Server::Server()
{
    if (TESTING)
        std::cout << "Default Server Constructor" << std::endl;
}

void Server::setAddress()
{
    //initialize the varibale with 0's
    std::memset(&_server_address, 0, sizeof(_server_address));
    //set the family type to IPv4
    _server_address.sin_family = AF_INET;
    //set the IP address using h to n long on the host unsigned int
    _server_address.sin_addr.s_addr = htonl(_pair.host);
    //set the port using h to n short on the port int.
    _server_address.sin_port = htons(_pair.port);
}


//set up the server with creating a socket, binding its fd and calling the listen function to make the fd ready for connections.
void Server::setup()
{
    //use socket to get an int (fd) associated with the socket
    //AF_INET makes out socket use IPv4 | SOCK_STREAM makes the socket a two-way relible commuinicator (TCP) | last argument is for the protocol number since there is only one it's 0.
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd == -1)
        throw SocketCreationError("Error in " + _name + " Server. Could not create a socket");
    //std::cout << "DEBUG: " << _name << " Socket is: " << _fd << std::endl;
    //fcntl(_fd, F_SETFL, O_NONBLOCK);
    //bind the socket with the socket fd, the address struct that was made in setAddress (needs to be casted to match bind type) and the size of adress struct
    if (bind(_fd, reinterpret_cast<struct sockaddr*>(&_server_address), sizeof(_server_address)) == -1)
        throw SocketBindingError("Error in " + _name + " Server. Could not bind a socket");
    if (listen(_fd, REQUEST_MAX) == -1)
        throw ListeningError("Error in " + _name + " Server. Could not make the socket listen");
}

int Server::accept()
{
    int client_socket;

    //accept a client connection. we only care about the fd of this conecction thus the 2nd and 3rd arguments are NULL.
    client_socket = ::accept(_fd, NULL, NULL);
    
    if (client_socket == -1)
        throw ClientConnectionError("Error in " + _name + " Server. Could not accept client connection");
    else
    {
        //make this fd non blocking fd. this helps the one poll() to run smoother.
        fcntl(client_socket, F_SETFL, O_NONBLOCK);
        //save this fd in the requests map, the string I.E the request sent from the client will be populated from the receive function
        _requests.insert(std::make_pair(client_socket, ""));
    }
    return (client_socket);
}

//recive funtion will read into the buffer. buffer wil be turned into std::string and be appended to the _requests string value of the matching client_fd.
//the send function clears the matching _request value this is why we apend to it so a request can be built up through multiple calls.
void Server::receive(int client_fd)
{
    char buffer[BUFFER_SIZE] = {0};
    int ret;
    
    ret = recv(client_fd, buffer, BUFFER_SIZE, 0);
    
    if (ret == 0 || ret == -1)
    {
        this->close(client_fd); //close the fd if a recv error occured.
        if (ret == 0)
            throw ClientConnectionError("Connection was closed by the client, in " + _name);
        else
            throw ClientConnectionError("Read error Connection closed in the servrver named: " + _name);
    }
    _requests[client_fd] += std::string(buffer);
}

//so far the functionallity of this server is echoing back with extra text
void Server::process(int client_fd)
{
    std::string response;
    
    response = _requests[client_fd];
    response += " -with love, the ";
    response += _name;
    response += " Server\n";
    _responses.insert(std::make_pair(client_fd, response));
}

//send function send the processed data by matching client_fd to _responses map.
void Server::send(int client_fd)
{
    std::string to_send;
    int ret;

    to_send = _responses[client_fd];
    ret = ::send(client_fd, to_send.c_str(), to_send.size(), 0);
    if (ret == -1)
    {
        this->close(client_fd); //closing the fd if there was a send error
        _responses.erase(client_fd);
        throw ClientConnectionError("Error while sending data in the server named: " + _name);
    }
    else
    {
        _requests[client_fd] = "";
        _responses.erase(client_fd);
    }
}

//fd closing function
void Server::close(int client_fd)
{
    if (client_fd > 0)
        ::close(client_fd);
    _requests.erase(client_fd); // always erase the _request matching the closing fd. server must not think there is a request from an fd that is closed.
}

//Has Response returns true if there is a response for a given fd.
bool Server::hasResponse(int client_fd)
{
    std::map<int, std::string>::iterator it;
    it = _responses.find(client_fd); // use .find() to find a matching key value.
    if (it == _responses.end())
        return false;
    it = _requests.find(client_fd);
    if (it == _requests.end())
        return false;
    if ((*it).second == "")
        return false;
    return true;
}


int Server::getFD() const 
{
    return (_fd);
}

t_host_port Server::getPair() const 
{
    return (_pair);
}

std::string Server::getName() const 
{
    return (_name);
}

std::ostream &operator<<(std::ostream& obj, Server const &server)
{
    obj << server.getName();
    obj << " FD: ";
    obj << server.getFD();
    return (obj);
}


const char * SocketCreationError::what() const throw()
{
    return (msg.c_str());
}

const char * SocketBindingError::what() const throw()
{
    return (msg.c_str());
}

const char * ListeningError::what() const throw()
{
    return (msg.c_str());
}

const char * ClientConnectionError::what() const throw()
{
    return (msg.c_str());
}
    
