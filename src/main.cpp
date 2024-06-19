/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleclerc <bleclerc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 14:52:28 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/19 13:13:45 by bleclerc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "Cluster.hpp"
void printConfig(Config conf);

int main(int argc, char *argv[], char **envp)
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
        Cluster cluster(configuration, envp);
        cluster.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
    
}
