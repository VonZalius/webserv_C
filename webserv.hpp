#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <string>
#include <fstream>

#include <sstream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>

#include <fcntl.h>

#include "Partie_C/PC_1.hpp"


// Simule une structure de configuration simple
struct ServerConfig
{
    int port = 8080;
    std::string basePath = "./the_ultimate_webserv";
    std::string index = "/index.html";
};

void main_C(int client_socket, ServerConfig& config);

//  http://localhost:8080
//  g++ -o server test.cpp
//  ./server 

#endif