/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRoutesConfig.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 18:29:19 by lkukhale          #+#    #+#             */
/*   Updated: 2024/05/12 16:33:05 by lkukhale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerRoutesConfig.hpp"

const char * InvalidGetCall::what() const throw()
{
   return (msg.c_str());
}

enum Rule{
   LOCATION,
   ALLOW_METHODS,
   ROOT,
   INDEX
};

bool ServerRoutesConfig::isRule(std::string input)
{
   for (std::vector<std::string>::const_iterator rule = ConfigBase::_rules.begin(); rule != ConfigBase::_rules.end(); rule++)
      if (input.compare(*rule) == 0)
         return (true);
   return (false);
}

void  ServerRoutesConfig::readRule(std::vector<std::string>::iterator input, std::vector<std::string>& route_block, std::vector<std::string>::const_iterator rule)
{
   //since the _rules vector of ConfigBase class is a constant pre defined vector we can just get the index of the rule we have found
   int rule_id = std::distance(ConfigBase::_rules.begin(), rule);
   bool http_rule_is_found;
   
   // using a switch statment (making it readable with enums) we can carry out each necaserry operation
   switch (rule_id)
   {
   //sub routes are under construction right now
   case LOCATION:
      std::cout << "under construction" << std::endl;
      break;
   //to do http methods we iterate over the following elements of the input vecotr and match each with a http methods pre defined in the ConfigBase class
   //we stop getting the allowed methods when an element matches none of the methods.
   //the only problem with this approach is that if an incorect method is typed all correct methods after it will be discarded, also this approach does not enforce the allowed_methods to be on the same line.
   case ALLOW_METHODS:
      while (true)
      {
         input++;
         http_rule_is_found = false;
         for (std::vector<std::string>::const_iterator it = ConfigBase::_http_methods.begin(); it != ConfigBase::_http_methods.end(); it++)
         {
            if ((*input).compare(*it) == 0)
            {
               _allowed_methods.push_back((*input));
               http_rule_is_found = true;
               break;
            }
         }
         if (!http_rule_is_found)
            break;
      }
      break;
   //this has the same problem of not enforcing the rule ("root") and the path ("./my/path") to be on the same line. 
   //also there is no checking if the path is valid but nginx also does not check...
   case ROOT:
      _root = *(++input);
      break;
   //since index is always declared inside a "{ }" block (route or server itself) the list of index files ends with either another rule or " }"
   // the problem with this approach is still no enforceing of single line inputs in the config file.  
   case INDEX:
      input++;
      while ((*input).compare("}") != 0 && !isRule(*input) &&  input != route_block.end())
      {
         _index_files.push_back(*input);
         input++;
      }
      break;
   
   default:
      break;
   }
   
}

ServerRoutesConfig::ServerRoutesConfig(std::vector<std::string> route_block, std::string location) : _index_files(), _allowed_methods()
{
   //the config file is split with a charset (space tab newline). must read info out from a vector like that here.
   //route block is whatever is inside "{ }" after the location is defined.
   //location is the path that is defined after the key word location and before the "{ }" block.
   _location = location;

   //iterate over all elements of the route block and for each check against the predefined set of rules, if its a match use the readRule function to fill in the member varibles of the config server.   
   for (std::vector<std::string>::iterator input = route_block.begin(); input != route_block.end(); input++)
      for (std::vector<std::string>::const_iterator rule = ConfigBase::_rules.begin(); rule != ConfigBase::_rules.end(); rule++)
         if ((*input).compare(*rule) == 0)
            readRule(input, route_block, rule);
   //this way if the same rules are declared multiple times the last one will override previous ones which is true to nginx's implementation
}

//could come up with some usual default values to use.
ServerRoutesConfig::ServerRoutesConfig()
{
   _root = "";
   _location = "";
   _index_files.push_back("index.html");
   _allowed_methods.push_back("ADD");  
}

ServerRoutesConfig::~ServerRoutesConfig()
{
   
}

ServerRoutesConfig& ServerRoutesConfig::operator=(const ServerRoutesConfig& rhs)
{
   _root = rhs.getRoot();
   _location = rhs.getLocation();
   _index_files = rhs.getIndex();
   _allowed_methods = rhs.getMethods();
}

std::string ServerRoutesConfig::getRoot() const
{
   return (_root);
}

std::string ServerRoutesConfig::getLocation() const
{
   return (_location);
}

std::vector<std::string> ServerRoutesConfig::getIndex() const
{
   return (_index_files);   
}

std::vector<std::string> ServerRoutesConfig::getMethods() const
{
   return (_allowed_methods);
}