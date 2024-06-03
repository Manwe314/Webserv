/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brettleclerc <brettleclerc@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 14:52:28 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/03 08:26:47 by brettlecler      ###   ########.fr       */
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

    if (argc != 2)
    {
        std::cout << "please only provide one file" << std::endl;
        return 0;
    }
    
    try
    {
        Config configuration(argv[1]);
		printConfig(configuration);
        Cluster cluster(configuration);
        cluster.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }



    return 0;
    
}
