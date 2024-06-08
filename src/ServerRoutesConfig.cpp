/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRoutesConfig.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 18:29:19 by lkukhale          #+#    #+#             */
/*   Updated: 2024/06/08 21:45:41 by lkukhale         ###   ########.fr       */
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
	INDEX,
   ERROR_PAGE
};

bool ServerRoutesConfig::isRule(std::string input)
{
	for (std::vector<std::string>::const_iterator rule = ConfigBase::_rules.begin(); \
	rule != ConfigBase::_rules.end(); rule++)
    	if (input.compare(*rule) == 0)
        	return (true);
	return (false);
}

void  ServerRoutesConfig::readRule(std::vector<std::string>::iterator input, \
std::vector<std::string>& route_block, std::vector<std::string>::const_iterator rule)
{
   //since the _rules vector of ConfigBase class is a constant pre defined vector we can just get the index of the rule we have found
   int rule_id = std::distance(ConfigBase::_rules.begin(), rule);
   std::vector<std::string> buffer;
   std::vector<int> status_codes;

   // using a switch statment (making it readable with enums) we can carry out each necaserry operation
   switch (rule_id)
   {
   //constructing sub routes is verry similar to serverconfig constructin subroutes. (see details in ServerConfig.cpp line 132)
   case LOCATION:
      std::pair<int, int> brackets = encapsule(route_block, "{", "}", std::distance(route_block.begin(), input));
      if (brackets.second != -1)
      {
         buffer.clear();
         for (int i = brackets.first; i <= brackets.second; i++)
            buffer.push_back(route_block[i]);
         _sub_routes.push_back(ServerRoutesConfig(buffer, *(++input)));
      }
      break;
   //to do http methods we iterate over the following elements of the input vecotr and match each with a http methods pre defined in the ConfigBase class
   //we stop getting the allowed methods when an element matches none of the methods.
   //the only problem with this approach is that if an incorect method is typed all correct methods after it will be discarded, also this approach does not enforce the allowed_methods to be on the same line.
   case ALLOW_METHODS:
      bool http_rule_is_found;
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

   case ERROR_PAGE:
      input++;
      while ((*input).compare("}") != 0 && !isRule(*input) &&  input != route_block.end())
      {
         if ((*input).find('/') == std::string::npos)
            status_codes.push_back(std::atoi((*input).c_str()));
         else
            break;
         input++;
      }
      if (!status_codes.empty())
         for (std::vector<int>::iterator it = status_codes.begin(); it != status_codes.end(); it++)
            _error_pages.insert(std::make_pair((*it), (*input)));
      break;
   
   default:
      break;
   }
   
}

ServerRoutesConfig::ServerRoutesConfig(std::vector<std::string> route_block, std::string location) : _index_files(), _allowed_methods(), _sub_routes(), _error_pages()
{
   //the config file is split with a charset (space tab newline). must read info out from a vector like that here.
   //route block is whatever is inside "{ }" after the location is defined.
   //location is the path that is defined after the key word location and before the "{ }" block.
   _location = location;
   std::pair<int, int> brackets;

   //iterate over all elements of the route block and for each check against the predefined set of rules, if its a match use the readRule function to fill in the member varibles of the config server.   
   for (std::vector<std::string>::iterator input = route_block.begin(); input != route_block.end(); input++)
      for (std::vector<std::string>::const_iterator rule = ConfigBase::_rules.begin(); rule != ConfigBase::_rules.end(); rule++)
      {
         if ((*input).compare(*rule) == 0)
         {
            readRule(input, route_block, rule);
            //to avoid double stacking some subroute configurations whnever a subroute is encoutered it is skipped after evaluateing it with readRule();
            if ((*input).compare("location") == 0)
            {
               brackets = encapsule(route_block, "location", "}", std::distance(route_block.begin(), input));
               if (brackets.second != -1)
                  input += (brackets.second - brackets.first);
            }
         }
      }
   //this way if the same rules are declared multiple times the last one will override previous ones which is true to nginx's implementation
}

//this function inherits from the "parent" route that is passed as an argument.
//this only happens if this route itself dosent aready have these values set.
void ServerRoutesConfig::inherit(ServerRoutesConfig parent)
{
   if (_root.empty())
      _root = parent.getRoot();
   if (_index_files.empty())
      _index_files = parent.getIndex();
   if (_allowed_methods.empty())
      _allowed_methods = parent.getMethods();
   if (_error_pages.empty())
      _error_pages = parent.getErrorPages();
   
}

//this function returns a struct that has an int and a pointer. the int represents the amount of characters that matched between uri and location.
// pointer is a pointer to the ServerRouteConfig that this match has happened in.
//this function will evaluate itself and all of its sub routes and return the BEST match I.E struct with the highest "int". (note that -1 means a compleate match which is considered as a higher number than any positive integer).
t_route_match ServerRoutesConfig::findRouteMatch(std::string uri)
{
   //match is evaluating iteslf
   t_route_match match;
   //vector of structs is for arbitrary amount of sub routes it may have.
   std::vector<t_route_match> sub_matches;

   //startint from the begining count how many chars match betwen location and uri before first missmatch.
   match.match_length = countMatchingChars(_location, uri); // ----> need to at logic for counting with slashes "/"  in mind <-----
   match.route = this;
   //if the matching length is same as the entire uri AND location path, then it is a compleate match. in this case no further searching is required since we cant find a better match.
   if (match.match_length == (int)uri.size() && match.match_length == (int)_location.size())
   {
      match.match_length = -1;
      return (match);
   }

   //if the route has NO nested subroutes then just return its own match.
   if (_sub_routes.empty())
      return (match);
   
   //loop over every subroute and use the same function for them to evaluate their best match.
   for (std::vector<ServerRoutesConfig>::iterator it = _sub_routes.begin(); it != _sub_routes.end(); it++)
   {
      //if this routes location is the begining of the subroutes location that means its a proper nesting. (for example: /banana/one nested under /potato is invalid, but /potato/one is valid)
      //in the case of inproper nesting the subroute just does not inherit from parent but regardless is evaluated for matching purposes. it is considered to be outside of the parent route block.
      if (countMatchingChars(_location, (*it).getLocation()) == (int)_location.size())
         (*it).inherit(*this);
      //use this same member function such that each sub route will return the struct with best match among THEIR subroutes. this will reqursively search every nested route.
      sub_matches.push_back((*it).findRouteMatch(uri));
   }
   
   //lastly find the best match among the sub routes and this route. 
   for (std::vector<t_route_match>::iterator it = sub_matches.begin(); it != sub_matches.end(); it++)
   {
      //if any subroute has found a perfect match. return it immediatly since a better match cant be found.
      if ((*it).match_length == -1)
      {
         match.match_length = -1;
         match.route = (*it).route;
         break;   
      }
      //in the if statement greater than is purposely used since if sub routes best match and this routes best match are the same we want to take the firs I.E patent route.
      if ((*it).match_length > match.match_length)
      {
         match.match_length = (*it).match_length;
         match.route = (*it).route;
      }
   }
   return (match);
}

//this function searches for root location route in its nested routes. this function is used by ServerConfig to find a root route of a server.
ServerRoutesConfig* ServerRoutesConfig::findRootRoute()
{
   ServerRoutesConfig* root;
   //if this route is root return this
   if (_location == "/")
      return this;  
   else if (!_sub_routes.empty()) //if this route has children
   {
      for (std::vector<ServerRoutesConfig>::iterator it = _sub_routes.begin(); it != _sub_routes.end(); it++)
      {
         root = (*it).findRootRoute(); //call this function of every child that will check their own children
         if (root != NULL) // a non null means that root route was found
            return (root);
      }
   }
   //if the for loop never returned then this means that root route is not nested in this route.
   return (NULL);
}

//Could come up with some usual default values to use.
ServerRoutesConfig::ServerRoutesConfig()
{
	_root = "";
	_location = "";
	_index_files.push_back("index.html");
	_allowed_methods.push_back("GET");  
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
   _sub_routes = rhs.getSubRoutes();
   _error_pages = rhs.getErrorPages();
   return (*this);
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

std::vector<ServerRoutesConfig> ServerRoutesConfig::getSubRoutes() const
{
   return (_sub_routes);
}

std::map<int, std::string> ServerRoutesConfig::getErrorPages() const
{
   return (_error_pages);  
}


std::ostream& operator<<(std::ostream& obj, ServerRoutesConfig const &conf)
{
   std::vector<std::string> help;
   std::vector<ServerRoutesConfig> aid;
   std::map<int, std::string> companion;
   
   obj << "Location: ";
   obj << conf.getLocation();
   obj << " root: ";
   obj << conf.getRoot();
   obj << "\n";
   help = conf.getIndex();
   if (!help.empty())
   {
      obj << "Index: ";  
      for (std::vector<std::string>::iterator i = help.begin(); i != help.end(); i++)
      {
         obj << *i;
         obj <<" ";
      }
      obj << "\n";
   }
   help = conf.getMethods();
   if (!help.empty())
   {
      obj << "Allowed Methods: ";  
      for (std::vector<std::string>::iterator i = help.begin(); i != help.end(); i++)
      {
         obj << *i;
         obj <<" ";
      }
      obj << "\n";
   }
   aid = conf.getSubRoutes();
   if (!aid.empty())
   {
      obj << "Amount of Sub Routes: ";
      obj << aid.size();
      obj << "\n";
   }
   companion = conf.getErrorPages();
   if (!companion.empty())
   {
      obj << "Error Pages: ";
      obj << "\n";
      for (std::map<int, std::string>::iterator i = companion.begin(); i != companion.end(); i++)
      {
         obj << (*i).first;
         obj << " - ";
         obj << (*i).second;
         obj << " ";
      }
      obj << "\n";
   }
   return (obj);
}
