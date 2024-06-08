/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 14:52:28 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/08 02:28:37 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "Cluster.hpp"
void printConfig(Config conf);

int main(int argc, char *argv[])
{
    ConfigBase base;
    std::vector<ServerConfig> servers;

    if (argc != 2)
    {
        std::cout << "please only provide one file" << std::endl;
        return 0;
    }
    
    try
    {
        Config configuration(argv[1]);
        servers = configuration.getServerConfigs();
		//printConfig(configuration);
        Cluster cluster(configuration);
        cluster.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
    
}
