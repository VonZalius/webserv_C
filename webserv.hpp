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


// Simule une structure de configuration simple
struct ServerConfig
{
    int port = 8080;
    std::string index = "the_ultimate_webserv/index.html";
};

void main_C(int client_socket);

//  http://localhost:8080
//  g++ -o server test.cpp
//  ./server 