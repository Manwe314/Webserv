/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brettleclerc <brettleclerc@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 23:27:16 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/25 19:11:36 by brettlecler      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
	400 - Bad request
	501 - Not Implemented
	505 - HTTP version not supported
*/

Response::Response(std::string request, ServerConfig config, t_host_port pair, char **envp) : _headers(), _pair(pair), _envp(envp)
{
    size_t i;
    _status_code = -1; //if the status code is -1 after this constructor finishes that means no errors were encountered.
    if (request.empty())
        _status_code = 400;
    _server_config = config;
    /*
		Here we need to split the request into 3 parts: request line, headers and body
    	and parse them individually according to the rules.
	*/
    i = request.find("\r\n");
    //if no CRLF was found set error status to 400.
    if (i == std::string::npos)
        _status_code = 400;
    else
        parseRequestLine(request.substr(0, i)); // prepare everything that is in the request line of the request.
    //message headers are sandwitched between request line and empty line. if empty line does not exist set error status to 400.
    if (i == std::string::npos || request.find("\r\n\r\n") == std::string::npos)
        _status_code = 400;
    else
        parseMessageHeaders(request.substr((i + 2), (request.find("\r\n\r\n") - (i + 2)))); // parse everything from the 2nd line I.E after the request line before the empty line.
    i = request.find("\r\n\r\n");
    if (i != std::string::npos && (i + 4) < request.size())
	{
		try
		{
			parseMessageBody(request.substr((i + 4)));
		}
		catch(TransferEncodingError & e)
		{
			std::cerr << e.what() << '\n';
			_status_code = 400;
		}
	}
}


//this function unfolds lines that are folded. 
//folding is when CRLF is followed by space or a tab. this is used to get big values on multiple lines.
//unfolding means replacing [CRLF 1*(SP | HT)] with just one space.
static void unfoldMessageHeaders(std::string &message_headers)
{
    int replace_size = 0;
    size_t i = 0;
    size_t j;

    while ((i = message_headers.find("\r\n", i)) != std::string::npos)
    {
        //i returned by find is at \r we add 2 to move to the next element after \r\n
        i += 2;
        //if i is not out of range and space or tab replace this run (starting from \r to the last consecutive space or tab) with just one space.
        if (i < message_headers.size() && (message_headers[i] == ' ' || message_headers[i] == '\t'))
        {
            //for str.replace() count the size of str needed to replace.
            j = i;
            while (j < message_headers.size() && (message_headers[j] == ' ' || message_headers[j] == '\t'))
            {
                j++;
                replace_size++;
            }
            //include the \r\n characters.
            replace_size += 2;
            message_headers.replace((i - 2), replace_size, " ");
            //since we removed CRLF and X amount of whitespace and inserted one space. to get to the same space we subtract 2 from i.
            i -= 2;
        }
    }
}


//this function assumes that the header line is correctly formated. it makes a key value pair where both are strings.
//the function returns empty strings if the feild-name of the header is not recognised.
static std::pair<std::string, std::string> makeHeaderPair(std::string header)
{
    std::string name;
    std::string value;
    bool is_recognised = false;

    //take everything before ":" colon. the substring is laready validated.
    //we will check it against a list of valid/implemented headers from ConfigBase class. since the name is case INSENSITIVE we lowercase the string first.
    name = header.substr(0, header.find(":"));
    toLowerCase(name);
    for (std::vector<std::string>::const_iterator it = ConfigBase::_valid_headers.begin(); it != ConfigBase::_valid_headers.end(); it++)
        if (name.compare(*it) == 0)
            is_recognised = true;
    if (!is_recognised)
        return (std::make_pair("", ""));
    //everything beyond ":" colon is feild-value which is returned in its form without extra parsing.
    value = header.substr((header.find(":") + 1));
    return (std::make_pair(name, value));
}
/*
	POST / HTTP/1.1
	Host: localfedfgw
	Transfer-encoding: chunked

	"4\r\nWiki\r\n"
	
	POST / HTTP/1.1
	Host: localfedfgw
	Transfer-encoding: chunked
	

	"5\r\npedia\r\n"


	POST / HTTP/1.1
	Host: localfedfgw
	Transfer-encoding: chunked
	

	"e\r\n in\r\n\r\nchunks.\r\n"


	POST / HTTP/1.1
	Host: localfedfgw
	Transfer-encoding: chunked
	
	
	"0\r\n\r\n"


	The first character is the chunked message size.
	If 0, then consider as the as the end of the entire message.
	The chunked size for a chunked message is always delimited by \r\n
	It will also be in hexadecimal format
	Thereafter read/stock the following content up until the aforementioned chunk size.
*/
void	Response::readChunkSize(std::string const & message_body)
{
	//find the chunk size delimiter /r/n
	size_t end_of_line_size = message_body.find("\r\n", _position);
	if (end_of_line_size == std::string::npos) //if not found, throw an error
		throw TransferEncodingError("Invalid chunk size format");
	
	//stock the chunk size in a string, to be converted later
	std::string	line_size = message_body.substr(_position, end_of_line_size - _position);
	_position = end_of_line_size + 2; //Move past the \r\n

	//convert the chunk size string, represented in a hexadecimal format 
	char *endptr;
	_chunk_size = std::strtoul(line_size.c_str(), &endptr, 16);
	if (*endptr != '\0') //ancient way to check for conversion errors, no choice - C++98
		throw TransferEncodingError("Invalid chunk size format");
}

void	Response::readChunkData(std::string const & message_body)
{
	/*
		Check if chunk size is at least smaller (if not equal) to the message size
		to avoid buffer overflow issues when appending
		No sure if required, the append fn probably does the job for us.
	*/
	if (_position + _chunk_size > message_body.size())
		throw TransferEncodingError("Invalid chunk data format");
	
	_body.append(message_body, _position, _chunk_size);

	//Move past the read chunked message
	_position += _chunk_size;
	/*
		This check ensures that the chunk size needs to be perfect.
		If the following characters after the appended data are not \r\n,
		then the data is not respecting the chunk size
	*/
	if (message_body.substr(_position, 2) != "\r\n")
		throw TransferEncodingError("Invalid chunk data format");
	_position += 2; //Move past the \r\n
}

//this function reads message body of the message.
void	Response::parseMessageBody(std::string message_body)
{
    int bytes_to_read = -1;
    std::map<std::string, std::string>::iterator it_length;
	std::map<std::string, std::string>::iterator it_chunk;

	it_length = _headers.find("content-length");
	it_chunk = _headers.find("transfer-encoding");

    if (_status_code != 400) //unless we already had an error.
    {
		/*
			HTTP/1.1 specification (RFC 7230, Section 3.3.3)
			A sender MUST NOT send a Content-Length header field in any message
			that contains a Transfer-Encoding header field.
		*/
		if (it_length != _headers.end() && it_chunk != _headers.end())
			throw TransferEncodingError("Content-length & transfer-encoding headers both present");
		//if transfer-encoding found and value is set to 'chunked'
		else if (it_chunk != _headers.end() && trimSpaces(it_chunk->second) == "chunked")
		{
			_position = 0;
			while (true) //loops over the chunked message until the chunk size is 0
			{
				_chunk_size = 0;
				readChunkSize(message_body); //gives us the chunked message size for reading
				if (_chunk_size == 0)
					break ;
				readChunkData(message_body); //reads as per the defined chunked size
			}
		}
		else if (it_length != _headers.end()) //if content_length found:
        {
            //get the value in an int.
            bytes_to_read = std::atoi(it_length->second.c_str());
            //if its under MAX_INT and non negative:
            if (bytes_to_read >= 0)
                _body = message_body.substr(0, bytes_to_read); //read that many bytes I.E characters.
            else
                _status_code = 400; //otherwise set error code to 400.
        }
		/*
			If we don't have content-length or transfer-encoding,
			take eveything after the "empty line" I.E what was passed as argument.
		*/
        else
            _body = message_body;
    }
}

//function to parse the headers part of the http request.
//the headers part is in the form of collection of lines. line[CRLF]...
//some lines have extra CRLF followed by space or tab to fold the line message into 2.
//after unfolding all the CRLF's left should properly denote lines thus we can split the string on these characters (since their single use is prohibited).
//the result will be a vector of strings that have 1 line each.
void Response::parseMessageHeaders(std::string message_headers)
{
    std::pair<std::map<std::string, std::string>::iterator, bool> result;
    std::vector<std::string> split_message_headers;
    std::pair<std::string, std::string> header;
    //unfold the message to remove extra CRLF's
    unfoldMessageHeaders(message_headers);
    //split it on CRLF.
    split_message_headers = split(message_headers, "\r\n");
    //go over each line, first validate it. if a single line is in invalid form error status is  400 and there is no reason to continiue.
    //if it is valid try to make the pair. if the name is not recognised empty "" will be returned, we must check for this. these lines will just be ignored.
    //lastly when inserting it to the member variable map check if the same name exists already, if it does append the value to it with a comma ", " and space ahead of it. 
    for (std::vector<std::string>::iterator it = split_message_headers.begin(); it != split_message_headers.end(); it++)
    {
        if(!isValidHeader(*it))
        {
            _status_code = 400;
            break;
        }
        else
            header = makeHeaderPair(*it);
        if (header.first != "")
        {
            result = _headers.insert(header);
            if (!result.second)
            {
                _headers[header.first] += ", ";
                _headers[header.first] += header.second;
            }
        }
    }   
}

char ** Response::appendToCharArray(char** array, int size, const char* new_element) 
{
    int i = 0;
    char** new_array = (char**)malloc((size + 2) * sizeof(char*));
    
    if (new_array == NULL) 
    {
        throw NoMatchFound("500");
    }

    
    while (i < size)
    {
        new_array[i] = ft_strdup(array[i]);
        i++;
    }
    new_array[i] = ft_strdup(new_element);
    if (new_array[i] == NULL)
    {
        free(new_array[i + 1]);
        freeCsplit(new_array);
        throw NoMatchFound("500");
    }
    new_array[i + 1] = NULL;

    return (new_array);
}

/*
	This function parses the request line of the message
	by convection space, new line and carrage return
	It should never be present in the request URI, method or http version.
	Therefore, we can split the string on these chars.
	The result from a valid request has 3 elements: method, URI and HTTP version.
*/
void Response::parseRequestLine(std::string request_line)
{
	std::vector<std::string> split_request_line = split(request_line, " \n\r");
    std::vector<std::string> split_uri;
    std::string query_variable;

    /*
        If the number of elements is not 3, the request is invalid.
    */
    if (split_request_line.size() != 3)
    {
        _status_code = 400;
        return ;
    }
    
    _method = split_request_line[0];
    _http_version = split_request_line[2];
    
    split_uri = split(split_request_line[1], "?");
    
    if (split_uri.size() > 2)
    {
        _status_code = 400;
        return ;
    }
    else if (split_uri.size() == 2)
    {
        _request_URI = split_uri[0];
        _query = split_uri[1];
        query_variable = "QUERY_STRING=" + _query;
        try
        {
            if (_envp != NULL)
                _envp = appendToCharArray(_envp, cArraySize(_envp), query_variable.c_str());
        }
        catch (const NoMatchFound &e)
        {
            _status_code = std::atoi(e.what());
            _query.clear();
            return ;
        }
    }
    else
    {
        _request_URI = split_request_line[1];
    }
    
    /*
        If the first part of the string is not an http method, the request is invalid.
    */
    if (!isValidHttpMethod(split_request_line[0]) && !isInvalidHttpMethod(split_request_line[0]))
    {
        _status_code = 400;
        return ;
    }
    /*
        If the first element is an http method but it isn't supported,
        the request is invalid with 501 (not 400).
    */
    if (isInvalidHttpMethod(split_request_line[0]))
    {
        _status_code = 501;
        return ;
    }
    /*
        If the version is not one of valid (supported) or invalid (not supported),
        the request is invalid.
    */
    if (!isValidVersion(split_request_line[2]) && !isInvalidVersion(split_request_line[2]))
    {
        _status_code = 400;
        return ;
    }
    /*
        If the request is one of invalid (unsupported) versions,
        the request is invalid with 505 (not 400).
    */
    if(isInvalidVersion(split_request_line[2]))
    {
        _status_code = 505;
        return ;
    }
}

Response::~Response()
{
    if (!_query.empty() && _envp != NULL)
		freeCsplit(_envp);
}

std::map<std::string, std::string> Response::getHeaders() const
{
    return (_headers);
}
ServerConfig Response::getServerConfig() const
{
    return (_server_config);
}
std::string Response::getHttpVersion() const
{
    return (_http_version);
}
std::string Response::getURI() const
{
    return (_request_URI);
}
std::string Response::getMethod() const
{
    return (_method);
}
std::string Response::getBody() const
{
    return (_body);   
}
int Response::getStatusCode() const
{
    return (_status_code);
}

std::string Response::getQuery() const
{
	return (_query);
}

const char * NoMatchFound::what() const throw()
{
	return (msg.c_str());
}

const char * TransferEncodingError::what() const throw()
{
	return (msg.c_str());
}

std::ostream& operator<<(std::ostream& obj, Response const &response)
{
    std::map<std::string, std::string> map = response.getHeaders();
    obj << "code: "; 
    obj << response.getStatusCode();
    obj << " method: "; 
    obj << response.getMethod();
    obj << " URI: ";
    obj << response.getURI();
    obj << " version: "; 
    obj << response.getHttpVersion();
    obj << "\nheaders:\n";
    for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); it++)
    {
        obj << "key: "; 
        obj << it->first;
        obj << " value: "; 
        obj << it->second;
        obj << "\n";
    }
    obj << "body: "; 
    obj << response.getBody();
	obj << "\n";
	obj << "query: ";
	obj << response.getQuery();
    return (obj);
}
