/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response-Process.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 16:59:31 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/05 19:43:33 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

std::string Response::handleErrorResponse()
{
    std::string error_response;
    
    if (_status_code / 100 != 4 || _status_code / 100 != 5)
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
}

std::string Response::process()
{
    std::string response;
    
    if (_status_code != -1)
    {
        response = handleErrorResponse();
        
    }
}