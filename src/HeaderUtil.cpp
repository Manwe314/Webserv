/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderUtil.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 19:26:24 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/21 02:05:00 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeaderUtil.hpp"

std::string getServerHeader()
{
    std::string server;
    
    server = "Server: webserv/1.0.0 \r\n";
    return (server);    
}

std::string getRetryHeader(int status, int seconds)
{
    std::string retry;
    if (status == 503 || status / 100 == 3)
    {
        retry = "Retry-After: ";
        retry += intToString(seconds);
        retry += " \r\n";
    }
    else
        retry = "";
    return (retry);
}

std::string getConnectionHeader()
{
    std::string connection;
    
    connection = "Connection: Keep-Alive \r\n";
    return (connection);
}

std::string getContentHeaders(size_t length, std::string path)
{
    size_t index;
    std::string content;
    std::string type;
    
    index = path.rfind(".");
    if (index != std::string::npos)
        type = path.substr(index + 1);
    if (isDirectory(path))
        type = "html";
    content = "Content-Length: ";
    content += sizetToString(length);
    content += " \r\n";
    content += "Content-Type: ";
    if (type == "html")
        content += "text/html";
    else if (type == "css")
        content += "text/css";
    else if (type == "js")
        content += "text/javascript";
    else if (type == "jepg" || type == "jpg")
        content += "image/jpeg";
    else if (type == "png")
        content += "image/png";
    else if (type == "bmp")
        content += "image/bmp";
    else if (type == "svg")
        content += "image/svg+xml";
    else
        content += "text/plain";
    content += " \r\n";
    return (content);
}

static std::string getDate()
{
    std::time_t raw_time;
    std::time(&raw_time);

    std::tm* gmt_time = std::gmtime(&raw_time);

    char  buffer[80];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt_time);
    return (std::string(buffer));
}

std::string getDateHeader()
{
    std::string date;

    date = "Date: ";
    date += getDate();
    date += " \r\n";
    return (date);
}
