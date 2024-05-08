/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 23:27:16 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/08 23:30:02 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(std::string request, ServerConfig config)
{
    if (request.empty())
        _status_code = 400;
    //will need to parse the request here.
    //will need an = overload or copy constructor for ServerConfig class.
    
}

Response::~Response()
{
    
}