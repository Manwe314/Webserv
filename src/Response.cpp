/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 23:27:16 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/18 21:13:34 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
400 - Bad request
501 - Not Implemented
505 - HTTP version not supported
*/

Response::Response(std::string request, ServerConfig config) : _headers()
{
    int i;
    _status_code = -1; //if the status code is -1 after this constructor finishes that means no errors were encountered.
    if (request.empty())
        _status_code = 400;
    _server_config = config;
    //here need to split the request into 3 parts: request line, headers and body.
    //parse them individually with according rules.
    //find the first occurance of CRLF (\r\n). everything before it is the request line.
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
        parseMessageBody(request.substr((i + 4)));
}


//this function unfolds lines that are folded. 
//folding is when CRLF is followed by space or a tab. this is used to get big values on multiple lines.
//unfolding means replacing [CRLF 1*(SP | HT)] with just one space.
static void unfoldMessageHeaders(std::string &message_headers)
{
    int replace_size = 0;
    int i = 0;
    int j;

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

//this fucntion reads message body based on content length of the message.
void Response::parseMessageBody(std::string message_body)
{
    int bytes_to_read = -1;
    std::map<std::string, std::string>::iterator it;
    
    if (_status_code != 400) //unless we already had an error.
    {
        //find the content-length pair in out _headers map
        it = _headers.find("content-length");
        if (it != _headers.end()) //if found:
        {
            //get the value in an int.
            bytes_to_read = std::atoi(it->second.c_str());
            //if its under MAX_INT and non negative:
            if (bytes_to_read >= 0)
                _body = message_body.substr(0, bytes_to_read); //read that many bytes I.E characters.
            else
                _status_code = 400; //otherwise set error code to 400.
        }
        else
            _body = message_body; //if we dont have content-length take eveything after the "empty line" I.E what was passed as argument.
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

//this function parses the request line of the message. by convection space, new line and carrage return
// should never be present in the request URI, method or http version. therefore we can split the string on these chars.
//the result in a valid request has 3 elements: method, URI and HTTP version.
void Response::parseRequestLine(std::string request_line)
{
    std::vector<std::string> split_request_line = split(request_line, " \n\r");

    if (split_request_line.size() != 3) //if the number of elements is not 3, the request is invalid.
    {
        _status_code = 400;
        return ;
    }
    if (!isValidHttpMethod(split_request_line[0]) && !isInvalidHttpMethod(split_request_line[0])) //if the first part of the string is not an http method, the rtequest is invalid.
    {
        _status_code = 400;
        return ;
    }
    if (isInvalidHttpMethod(split_request_line[0])) // if the first element is an http method but its not supported, the request is invalid with 501 (not 400).
    {
        _status_code = 501;
        return ;
    }
    if (!isValidVersion(split_request_line[2]) && !isInvalidVersion(split_request_line[2]))//if the version is not one of valid (supported) or invalid (not supported), the request is invalid.
    {
        _status_code = 400;
        return ;
    }
    if(isInvalidVersion(split_request_line[2])) //if the request is one of invalid (unsupported) versions, the request is invalid with 505 (not 400).
    {
        _status_code = 505;
        return ;
    }
    _method = split_request_line[0];
    _request_URI = split_request_line[1];
    _http_version = split_request_line[2];
}

Response::~Response()
{
    
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

std::ostream& operator<<(std::ostream& obj, Response const &response)
{
    std::map<std::string, std::string> map = response.getHeaders();
    obj << "code: "; 
    obj << response.getStatusCode();
    obj << "method: "; 
    obj << response.getMethod();
    obj << "URI: ";
    obj << response.getURI();
    obj << "version: "; 
    obj << response.getHttpVersion();
    obj << "headers\n";
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
    return (obj);
}
