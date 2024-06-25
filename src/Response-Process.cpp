/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response-Process.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 16:59:31 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/25 03:49:59 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Response.hpp"
#include "HeaderUtil.hpp"

//the most default 404 http response that is hard coded so if everything else fails there is at leas something to respond with.
std::string Response::default404()
{
    std::string response;

    response = "HTTP/1.1 404 Not Found \r\nServer: webserv/1.0.0 \r\nContent-Type: text/html \r\nContent-Length: 470 \r\nConnection: Keep-Alive \r\n";
    response += getDateHeader();
    response += "\r\n";
    response += "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>404 Not Found Default.</title>\n<style>\nbody { font-family: Arial, sans-serif; text-align: center; padding: 50px; }\nh1 { font-size: 50px; }\np { font-size: 20px; }\n</style>\n</head>\n<body>\n<h1>404 Not Found</h1>\n<p>The page you are looking for might have been removed, had its name changed, or is temporarily unavailable.</p>\n</body>\n</html>";
    return (response);
}

//this function builds and returns the headers feild of a http response
std::string Response::headersProcess(std::string body, std::string path)
{
    std::string headers;
    
    headers += getServerHeader();
    headers += getDateHeader();
    headers += getRetryHeader(_status_code, TIMEOUT_SEC);
    headers += getConnectionHeader();
    if (body != "")
        headers += getContentHeaders(body.size(), path);
    return (headers);
}

//this function builds and returns the status line of a http response
std::string Response::statusLineProcess()
{
    std::string status_line;
    std::map<int, std::string>::const_iterator it;

    it = ConfigBase::_reason_phrases.find(_status_code);
    if (it == ConfigBase::_reason_phrases.end())
        throw NoMatchFound("500");

    status_line = "HTTP/1.1 ";
    status_line += intToString(_status_code);
    status_line += " ";
    status_line += (*it).second;
    status_line += " \r\n";
    return (status_line);
}

//this fucntion takes in the URI and returns a ServerRoutesConfig object with all the relevant directives.
ServerRoutesConfig Response::matchSubRoute(std::string uri)
{
    std::string location;
    ServerRoutesConfig* match;
    t_route_match temp;
    t_route_match save;
    std::vector<ServerRoutesConfig> routes;

    if (uri.empty())
        throw NoMatchFound("400");

    routes = _server_config.getRouteConfigs();
    save.match_length = 0;
    
    //loop over the routes of this server with ServerRoutesConfig's member function that also checks all of their nested routes.
    for (std::vector<ServerRoutesConfig>::iterator it = routes.begin(); it < routes.end(); it++)
    {
        //the member function returns a struct with the information about the BEST match it could find among itself and its children
        //match_length specifies how good of a match it was. with -1 meaning exact match.
        //route is a pointer to the object that had match_length amount of characters matching
        (*it).inherit(_server_config.getServerWideConfig());
        temp = (*it).findRouteMatch(uri, _server_config.getServerWideConfig());
        //if we find a perfect match, since we are looping from the bbegining its the perfect match written first. we no longer search.
        if (temp.match_length == -1)
        {
            save.match_length = temp.match_length;
            save.route = temp.route;
            break;
        }
        //if perfect match is not found save the beast match.
        if (temp.match_length > save.match_length)
        {
            save.match_length = temp.match_length;
            save.route = temp.route;
        }
    }

    //if uri does not start with "/" and some locatiosn are incorrectly declared, a match couldve been found, but the request is still invalid
    if (uri[0] != '/')
        throw NoMatchFound("400");
    
    //if the best match was 0 and uri Starts with "/" that would mean that ALL routes are decalred incorrectly so 404 not found should be returned
    if (save.match_length == 0)
        throw NoMatchFound("404");


    //if best match is 1 and uri starts with "/", then this means that only the "/" of locations and uri matched, in this case we need to find the root location I.E location with only "/" 
    if (save.match_length == 1)
    {
        match = _server_config.findRootSubRoute();
        //if root route is not found send 404 not found.
        if (match == NULL)
            throw NoMatchFound("404");
        //since root location can never actualy be nested it should always be considered as highest level route (right after the server) and it must inherit the directives of the server
        (*match).inherit(_server_config.getServerWideConfig());
        return (*match);
    }
    
    match = save.route;
    return (*match);
}

std::string Response::makeFullPath(ServerRoutesConfig config, std::string uri)
{
    //alias directivve functionality could be added here
    std::string path;
    
    if (uri[0] != '/')
        throw NoMatchFound("400");

    if (config.getRoot().empty() && !config.getAlias().empty())
    {
        if (countMatchingChars(config.getLocation(), uri) != (int)config.getLocation().size())
            throw NoMatchFound("404");
        path = config.getAlias();
        uri.erase(0, config.getLocation().size());
    }
    else if (!config.getRoot().empty() && config.getAlias().empty())
        path = config.getRoot();
    else if (config.getRoot().empty() && config.getAlias().empty())
        throw NoMatchFound("404");
    else if (!config.getRoot().empty() && !config.getAlias().empty())
    {
        if (countMatchingChars(config.getLocation(), uri) == (int)config.getLocation().size())
        {
            path = config.getAlias();
            uri.erase(0, config.getLocation().size());
        }
        else
            path = config.getRoot();
    }    
    if (path[(path.size() - 1)] == '/' && uri[0] == '/')
        path.erase(path.size() - 1, 1);
    path += uri;
    return (path);
}

//this function returns the std::string response according to error code, or an empty string if the status cod eis not an error (4XX or 5XX)
std::string Response::handleErrorResponse()
{
    std::string error_response;
    std::string status_line;
    std::string headers;
    std::string body;
    ServerRoutesConfig config;
    
    if (_status_code / 100 != 4 && _status_code / 100 != 5)
        return (std::string(""));
    try
    {
        /*
            1. get the relevand config by matching the URI with a route
            2. get the Status line (http version SP code SP reason phrase CRLF)
            3. get the relevant body I.E html page. the ServerRoutesConfig's member function serveCustomError(int)
                will return the custom error page filepath if there is one or default error page if there is no custom error.
            4. get the headers (key: value CRLF);
            5. build the response by stiching status line and headers, adding an empty line and then the body.
        */
        config = matchSubRoute(_request_URI);
        status_line = statusLineProcess();
        body = readFile(config.serveCustomError(_status_code));
        headers = headersProcess(body, config.serveCustomError(_status_code));
        error_response = status_line + headers + "\r\n" + body;
    }
    catch(const NoMatchFound& e)
    {
        //if a matching route cant be found use serverwide configuration.
        config = _server_config.getServerWideConfig();
        try
        {
            //route matching's failure results in eaither 404 or 400. e.what() equals one of these values in string form. (if status line fails it equals 500)
            _status_code = std::atoi(e.what());
            status_line = statusLineProcess();
            body = readFile(config.serveCustomError(std::atoi(e.what())));
            headers = headersProcess(body, config.serveCustomError(std::atoi(e.what())));
            error_response = status_line + headers + "\r\n" + body;
        }
        catch(const CouldNotOpenFile& e)
        {
            //if the error file cant be opened use a hard coded 404 message.
            error_response = default404();
        }
    }
    catch(const CouldNotOpenFile& e)
    {
        error_response = default404();
    }
    return error_response;
}

std::string Response::serviceGetResource(ServerRoutesConfig config, std::string uri)
{
    std::string body;
    std::string path;
    std::string type;
    std::vector<std::string> index;

    path = uri;
    
    if (pathStatus(uri) == IS_DIRECTORY)
    {
        if (uri[uri.size() - 1] != '/')
            uri += "/";
        index = config.getIndex();
        if (!index.empty())
        {
            for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); it++)
            {
                path = uri + (*it);
                if (pathStatus(path) == IS_FILE)
                    break;
            }
        }
        else
        {
            if (config.getAutoindex() == 1)
            {
                body = listDirectoryContents(uri, _pair);
                if (body.empty())
                    throw NoMatchFound("403");
                _path = uri;
                return (body);
            }
            else
                throw NoMatchFound("403");
        }
    }

    if (pathStatus(path) == DOES_NOT_EXIST)
        throw NoMatchFound("404");
    if (pathStatus(path) == PERMISSION_DENIED)
        throw NoMatchFound("403");
    type = path.substr(path.rfind(".") + 1);
    try
    {
        if (type == "jpeg" || type == "jpg" || type == "png" || type == "bmp" || type == "svg")
        {
            body = readBinaryFile(path);
        }
        else
        {
            body = readFile(path);
        }
        _path = path;
    }
    catch(const CouldNotOpenFile& e)
    {
        throw NoMatchFound("404");
    }
    return (body);
}

std::string Response::processGET()
{
    ServerRoutesConfig config;
    std::string response;
    std::string status_line;
    std::string headers;
    std::string body;
    
    try
    {
        config = matchSubRoute(_request_URI);
        if (!isAllowed(config.getMethods() ,_method))
        {
            _status_code = 405;
            return (handleErrorResponse());
        }
        body = serviceGetResource(config, makeFullPath(config, _request_URI));
        _status_code = 200;
        headers = headersProcess(body, _path);
        status_line = statusLineProcess();
        response = status_line + headers + "\r\n" + body;
    }
    catch(const NoMatchFound& e)
    {
        _status_code = std::atoi(e.what());
        response = handleErrorResponse();
    }
    return (response);
}

std::string Response::processDELETE()
{
    ServerRoutesConfig config;
    std::string response;
    std::string status_line;
    std::string headers;
    std::string body;
    std::string path;

    try
    {
        config = matchSubRoute(_request_URI);
        path = makeFullPath(config, _request_URI);
        if (!isAllowed(config.getMethods(), _method))
        {
            _status_code = 405;
            return (handleErrorResponse());
        }
        if (pathStatus(path) == IS_FILE)
        {
            if (remove(makeFullPath(config, _request_URI).c_str()) == 0)
                _status_code = 204;
            else
            {
                _status_code = 500;
                return (handleErrorResponse());
            }
        }
        else if (pathStatus(path) == PERMISSION_DENIED)
        {
            _status_code = 403;
            return (handleErrorResponse());
        }
        else if (pathStatus(path) == DOES_NOT_EXIST)
        {
            _status_code = 404;
            return (handleErrorResponse());
        }
        body = "";
        status_line = statusLineProcess();
        headers = headersProcess(body, _request_URI);
        response = status_line + headers + "\r\n";
    }
    catch(const NoMatchFound& e)
    {
        _status_code = std::atoi(e.what());
        response = handleErrorResponse();
    }
    return (response);
}

static bool hasUnimplementedContent(std::map<std::string, std::string> headers)
{
    std::map<std::string, std::string>::iterator it;

    it = headers.find("content-encoding");
    if (it != headers.end())
        return (true);
    it = headers.find("content-md5");
    if (it != headers.end())
        return (true);
    it = headers.find("content-range");
    if (it != headers.end())
        return (true);
    return (false);
    
}

std::string Response::processPUT()
{
    ServerRoutesConfig config;
    std::string path;
    std::string status_line;
    std::string headers;
    std::string response;

    try
    {
        config = matchSubRoute(_request_URI);
        path = makeFullPath(config, _request_URI);
        if (!isAllowed(config.getMethods(), _method))
        {
            _status_code = 405;
            return (handleErrorResponse());
        }
        if (hasUnimplementedContent(_headers))
        {
            _status_code = 500;
            return (handleErrorResponse());
        }
        if (pathStatus(path) == IS_DIRECTORY)
        {
            _status_code = 409;
            return (handleErrorResponse());
        }
        if (pathStatus(path) == PERMISSION_DENIED)
        {
            _status_code = 403;
            return (handleErrorResponse());
        }
        if (pathStatus(path) == IS_FILE)
        {
            std::ofstream file(path.c_str(), std::ios::out);
            if (!file)
            {
                _status_code = 403;
                return (handleErrorResponse());
            }
            file << _body;
            file.close();
            _status_code = 204;
        }
        else
        {
            std::ofstream file(path.c_str(), std::ios::out);
            if (!file)
            {
                _status_code = 500;
                return (handleErrorResponse());
            }
            file << _body;
            file.close();
            _status_code = 201;
        }
        status_line = statusLineProcess();
        headers = headersProcess("", "");
        response = status_line + headers + "\r\n";
    }
    catch(const NoMatchFound& e)
    {
        _status_code = std::atoi(e.what());
        return (handleErrorResponse());
    }
    return (response);
}

std::string Response::process()
{
    std::string response;
    
    if (_status_code != -1)
    {
        response = handleErrorResponse();
        return response;
    }
    if (_method == "GET")
        response = processGET();
    else if (_method == "DELETE")
        response = processDELETE();
    else if (_method == "PUT")
        response = processPUT();
    else
    {
        std::cout << YELLOW << "\n(NOT A GET REQUEST)" << DEFAULT << std::endl;
        response = default404();
    }
    return response;
}