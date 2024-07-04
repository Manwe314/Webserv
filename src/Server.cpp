/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 16:12:04 by lkukhale          #+#    #+#             */
/*   Updated: 2024/07/04 16:00:44 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
    For now, the Server class is set up just so the i/o multiplexing works.
	It has 0 http functionallity (no testing has been done yet).
    Cluster under construction.
*/
#include "Server.hpp"
#include "Response.hpp"

//initialize the class memeber variables. including the requests map to an empty map.
Server::Server(t_host_port pair, std::string name ,int fd, ServerConfig config, std::vector<ServerConfig> alt) : _requests(), _responses(), _alternative_configs(alt)
{
    _pair.host = pair.host;
    _pair.port = pair.port;
    _name = name;
    _fd = fd;
    _config = config;
    //populate the server address struct
    this->setAddress();
}

Server::~Server() 
{
    
}

Server::Server()
{
    if (DEBUG)
        std::cout << "Default Server Constructor" << std::endl;
}

void Server::setAddress()
{
    //Initialize the variable with '0's
    std::memset(&_server_address, 0, sizeof(_server_address));
    //Set the family type to IPv4
    _server_address.sin_family = AF_INET;
    //Set the IP address using h to n long on the host unsigned int
    _server_address.sin_addr.s_addr = htonl(_pair.host);
    //Set the port using h to n short on the port int.
    _server_address.sin_port = htons(_pair.port);
}

/*
	Set up the server by creating a socket, binding its fd
	and calling the listen function to make the fd ready for connections.
*/
void Server::setup()
{
	/*
    	Use the socket to get an int (fd) associated with the socket.
    	AF_INET makes the socket use IPv4
		SOCK_STREAM makes the socket a two-way relible commuinicator (TCP)
		Last argument is for the protocol number. Since there is only one, it's set at 0.
	*/
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd == -1)
        throw SocketCreationError("Error in " + _name + " Server. Could not create a socket");
    //std::cout << "DEBUG: " << _name << " Socket is: " << _fd << std::endl;
    //fcntl(_fd, F_SETFL, O_NONBLOCK);
    /*
		Bind the socket with the socket fd
		with the address struct that was created in setAddress
		(needs to be casted to match bind type) and the size of address struct
	*/
    if (bind(_fd, reinterpret_cast<struct sockaddr*>(&_server_address), sizeof(_server_address)) == -1)
        throw SocketBindingError("Error in " + _name + " Server. Could not bind a socket");
    if (listen(_fd, REQUEST_MAX) == -1)
        throw ListeningError("Error in " + _name + " Server. Could not make the socket listen");
}

int Server::accept()
{
    int client_socket;

	/*
    	Accepts a client connection.
		We only care about the fd of this connection.
		Thus the 2nd and 3rd arguments are NULL.
	*/
    client_socket = ::accept(_fd, NULL, NULL);
    
    if (client_socket == -1)
        throw ClientConnectionError("Error in " + _name + " Server. Could not accept client connection");
    else
    {
        //Make this fd a non-blocking fd. This helps the one poll() to run smoother.
        fcntl(client_socket, F_SETFL, O_NONBLOCK);
        /*
			Save this fd in the requests map.
			The string I.E the request sent from the client will be populated
			in the receive function
		*/
        _requests.insert(std::make_pair(client_socket, ""));
    }
    return (client_socket);
}

bool	Server::isChunkedMessage( std::string const & buffer )
{
	Response temp(buffer, _config, _pair, NULL);
    std::map<std::string, std::string> headers = temp.getHeaders();
    std::map<std::string, std::string>::iterator it = headers.find("transfer-encoding");
    std::string::iterator end_pos;
    std::string chunk_value;

    if (it == headers.end())
        return (false);
	else
	{
		chunk_value = (*it).second;
		end_pos = std::remove(chunk_value.begin(), chunk_value.end(), ' ');
		chunk_value.erase(end_pos, chunk_value.end());
		if (chunk_value == "chunked")
			return (true);
		else
			return (false);
	}
	return (false);
}

/*
	Changed the parsing from a split-based parsing
	to a classic iteration parsing, since I wanted to add the \n characters
	to the body and the split fn was doing too good a job.
*/
std::string	Server::parseChunkedMessage( int client_fd )
{
	std::string				raw_data;
	std::string				headers;
	std::string				parsed_body = "";
	std::string::size_type	pos, chunk_size_pos;
	long					chunk_size;
	char					*endptr;

	raw_data = _chunked[client_fd];
	//Determine header part
	pos = raw_data.find("\r\n\r\n");
	if (pos == std::string::npos)
		throw TransferEncodingError("Invalid chunk size format: header separation not present");
	//Move past the \r\n\r\n
	pos += 4;
	try {
		headers = raw_data.substr(0, pos);
		raw_data = raw_data.substr(pos);
	}
	catch (const std::out_of_range& e)
	{
		throw TransferEncodingError("Invalid chunk size format: wrong header and body format");
	}
	// Start parsing the raw body
	pos = 0;
	while (true)
    {
        // Find the position of the next \r\n
        chunk_size_pos = raw_data.find("\r\n", pos);
        if (chunk_size_pos == std::string::npos)

            throw TransferEncodingError("Invalid chunk size format: missing CRLF after chunk size");

        // Extract the chunk size string
        std::string chunk_size_str = raw_data.substr(pos, chunk_size_pos - pos);
        
        // Convert the chunk size string to a long integer
        chunk_size = strtol(chunk_size_str.c_str(), &endptr, 10); // Base 10 for decimal conversion

        // Check for conversion errors
        if (*endptr != '\0' || endptr == chunk_size_str.c_str() || chunk_size < 0)
            throw TransferEncodingError("Invalid chunk size format: non-numeric or negative chunk size");

        pos = chunk_size_pos + 2; // Move past the \r\n

        if (chunk_size == 0)
            break; // Last chunk (0) found, exit the loop

        // Ensure there's enough data for the chunk and the following \r\n
        if (pos + chunk_size + 2 > raw_data.size())
            throw TransferEncodingError("Invalid chunk size format: not enough data for declared chunk size");

        // Append the chunk to the parsed body
        parsed_body += raw_data.substr(pos, chunk_size);
        pos += chunk_size;

        // Ensure the chunk is followed by \r\n
        if (raw_data.substr(pos, 2) != "\r\n")
            throw TransferEncodingError("Invalid chunk size format: chunk not properly terminated");
		else
			parsed_body += "\r\n"; //Add the \r\n to the body for a cleaner format
        
        pos += 2; // Move past the \r\n
    }
	//std::cout << BLUE << headers + parsed_body << DEFAULT << std::endl;
	return (headers + parsed_body);
}

ServerConfig Server::determineServer(std::string request)
{
    Response temp(request, _config, _pair, NULL);
    std::map<std::string, std::string> headers = temp.getHeaders();
    std::map<std::string, std::string>::iterator it = headers.find("host");
    std::string::iterator end_pos;
    std::string host_name;
    if (it == headers.end())
        return (_config);
    if (_alternative_configs.empty())
        return (_config);
    host_name = (*it).second;
    end_pos = std::remove(host_name.begin(), host_name.end(), ' ');
    host_name.erase(end_pos, host_name.end());
    for (std::vector<ServerConfig>::iterator i = _alternative_configs.begin(); i != _alternative_configs.end() ; i++)
    {
        if ((*i).getName() == host_name)
        {
            return (*i);
        }
    }
    return (_config);
}

/*
	The receive function will read into the buffer.
	The buffer will be turned into std::string
	and be appended to the _requests string value of the matching client_fd.
	
	The send function clears the matching _request value.
	This is why we append to it so a request can be built up through multiple calls.
*/
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
			throw ClientConnectionError("Read error: Connection closed in the server named: " + _name);
	}
	if (_chunked.find(client_fd) != _chunked.end() || isChunkedMessage(std::string(buffer, ret)))
	{
		if (_chunked.find(client_fd) == _chunked.end())
			_chunked.insert(std::make_pair(client_fd, std::string(buffer, ret)));
		else
			_chunked[client_fd] += buffer;
		if (std::string(buffer, ret).find("0\r\n\r\n") != std::string::npos)
		{
			try
			{
				_requests[client_fd] = parseChunkedMessage(client_fd);
			}
			catch(TransferEncodingError & e)
			{
				std::cerr << e.what() << std::endl;
				this->close(client_fd); //closing the fd if there was an encoding error
				_chunked.erase(client_fd);
				throw ClientConnectionError("Error while receiving chunked data in the server named: " + _name);
			}
			_chunked.erase(client_fd);
		}
	}
	else
	{
		_requests[client_fd] += std::string(buffer, ret);
	}
	if (DEBUG)
	{
		std::cout << YELLOW << "\n~~~~~~~~~~~~~message~~~~~~~~~~~~~" << DEFAULT <<std::endl;
		std::cout << _requests[client_fd] << std::endl;
		std::cout << YELLOW << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << DEFAULT << std::endl;
	}
}

void Server::process(int client_fd, char **envp)
{
    ServerConfig config;
	std::map<int, std::string>::iterator it;
	/*
		For some reason, the client_fd always gets added to the _request map
		So I have to add the second condition (similar to that of send fn).
		Commenting the else part allows the chunking messages to work properly
		But keeping it commented in case we have other issues popping up
	*/

	it = _requests.find(client_fd);
    if (it != _requests.end() && (*it).second != "")
	{
		config = determineServer(_requests[client_fd]);
		Response response(_requests[client_fd], config, _pair, envp);
		
		if (DEBUG)
			std::cout << MAGENTA << "THE RESPONSE object:\n" << response << DEFAULT << std::endl;
		std::string responseio = response.process();
		_responses.insert(std::make_pair(client_fd, responseio));
		if (DEBUG)
			std::cout << CYAN << "THE RESPONSE msg:\n" << responseio << DEFAULT << std::endl;
	}
	// else
	// {
	// 	_responses.insert(std::make_pair(client_fd, ""));
	// 	return ;
	// }
}

//The send function sends the processed data by matching client_fd to _responses map.
void Server::send(int client_fd)
{
    std::string to_send;
    std::map<int, std::string>::iterator it;
    int ret;

	it = _responses.find(client_fd);
    if (it != _responses.end() && (*it).second != "")
    {
		if (DEBUG)
        	std::cout << LAVENDER << "SENDING DATA" << DEFAULT << std::endl;
        to_send = (*it).second;
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
}

//fd closing function
void Server::close(int client_fd)
{
    if (client_fd > 0)
        ::close(client_fd);
	/*
		Always erase the _request matching the closing fd.
		The server must not think there is a request from an fd that has been closed.
	*/
    _requests.erase(client_fd);
}

//The Has Response returns true if there is a response for a given fd.
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

//Receive() prints
//std::cout << LAVENDER << "\n*************alters*************\n" << DEFAULT << std::endl;
//std::cout << GREEN << _pair << " " << _name <<DEFAULT << std::endl;
//printVector(_alternative_configs);
//std::cout << LAVENDER << "********************************" << DEFAULT << std::endl;