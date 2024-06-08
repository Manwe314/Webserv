/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response-Process.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 16:59:31 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/08 03:23:09 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Response.hpp"

//this fucntion takes in the URI and returns a ServerRoutesConfig object with all the relevant directives.
ServerRoutesConfig Response::matchSubRoute(std::string uri)
{
    std::string location;
    ServerRoutesConfig* match;
    t_route_match temp;
    t_route_match save;
    std::vector<ServerRoutesConfig> routes;

    routes = _server_config.getRouteConfigs();
    save.match_length = 0;
    
    //loop over the routes of this server with ServerRoutesConfig's member function that also checks all of their nested routes.
    for (std::vector<ServerRoutesConfig>::iterator it = routes.begin(); it < routes.end(); it++)
    {
        //the member function returns a struct with the information about the BEST match it could find among itself and its children
        //match_length specifies how good of a match it was. with -1 meaning exact match.
        //route is a pointer to the object that had match_length amount of characters matching
        temp = (*it).findRouteMatch(uri);
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

std::string Response::handleErrorResponse()
{
    std::string error_response;
    ServerRoutesConfig config;
    
    if (_status_code / 100 != 4 && _status_code / 100 != 5)
        return (std::string(""));
        /*
        if the request URI is intangable use default error pages path defined by DEFAULT_ERROR. and in this case its always 400 so its 
        DEFULT_ERROR + "/400_err.html" file. 

        next case would be an error with a valid URI so match it to a location.
        locations nested add their paths. correct location is one that matches THE MOST. use its root or any root defined above it. 
        
        try to make location matching as uneversal as possible to reuse it multiple times. 

        need to add error_page handling in the config parsing part to get custom set error pages if needed.
        it is possible for user to set a custom error page that then is not found in that case the response turns into a new 404 not found error
        
        if there is no custom error page set use a defualt one. if that is also not found default to 404 again.
        this 404 willl be hard coded as a string in code so at the very least it will always be avalable to show.

        after the apropriate file is found we will also need to make the appopriate headers:

        date:, server:, content-type:, content-lenght, connection, retry-after:. 

        after all the headers are made we need to make the status line which is: HTTP vesrion sp code sp text CRLF

        to make text easy we can use CONFIGBASE class since there is a preset amount of code we can get and their text is predefined. 

        adding an empty line CRLFCRLF at the end of headers compleates the error response. 
        */

    try
    {
        /*
            so far matching is working. currantly i know of 2 issues that need fixing.
            1. when matching uri to location we just count how many chars match. this is not correct since if location is /HEYBOB 
                and the uri is /HEYALICE the uri would match the location up to 4 chars, yet this is not correct since its just the 
                begining of the 2 folders names that match. 
            2. when searching through the tree of nested subroutes. we need to inherit the rules of correctly nested routes like:
                /one/two beeing under /one and NOT /three/four beeing under /one. both nested routes should end up inheriting from serverwide directives
                but, in the case of inccorect nesting the nested route is considered to be highest level, so that one should just inherit from
                serverwide. correctly nested route should inherit from its parent that has inherited from serverwide. this is important since
                if the parent and the serverwide have smae directives but the child doesnt the child should get its parents version and not the serverwide one.
        */
        if (!_request_URI.empty())
            config = matchSubRoute(_request_URI);
    }
    catch(const NoMatchFound& e)
    {
        std::cout << e.what() << std::endl;
        /*
        if (e.what() == "400")
          do deafult error page stuff here
        if (e.what() == "404")
            do default error page stuff here;  
        */
    }
    error_response = "boobya\n";
    return error_response;
}

std::string Response::process()
{
    std::string response;
    
    if (_status_code != -1)
    {
        response = handleErrorResponse();
    }
    response = "booya\n";
    return response;
}