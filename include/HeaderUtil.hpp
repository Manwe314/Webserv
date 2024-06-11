/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderUtil.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 19:25:06 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/11 20:57:40 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADERUTIL_HPP
#define HEADERUTIL_HPP

#include "Webserv.hpp"
#include "Config.hpp"

std::string getDateHeader();
std::string getContentHeaders(size_t length, std::string path);
std::string getConnectionHeader();
std::string getRetryHeader(int status, int seconds);
std::string getServerHeader();


#endif
