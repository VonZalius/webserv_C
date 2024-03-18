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

#define PORT 8080

// Simule une structure de configuration simple
struct ServerConfig
{
    int port;
    std::string index = "the_ultimate_webserv/index.html";

    std::string welcomeMessage;
    std::string httpResponse_blue; 

    /*"HTTP/1.1 200 OK\n"
    "Content-Type: text/html\n\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "<title>Page Title</title>\n"
    "<style>\n"
    "body { background-color: lightblue; }\n"
    "h1 { color: white; text-align: center; }\n"
    "p { font-family: verdana; font-size: 20px; }\n"
    "button {\n"
    "  font-size: 20px;\n"
    "  padding: 10px 20px;\n"
    "  background-color: #f44336;\n"
    "  color: white;\n"
    "  border: none;\n"
    "  cursor: pointer;\n"
    "  margin: 20px;\n"
    "}\n"
    "button:hover {\n"
    "  background-color: #d7372f;\n"
    "}\n"
    "</style>\n"
    "</head>\n"
    "<body>\n"
    "<h1>Hello from Partie C!</h1>\n"
    "<p>This is a simple web page with a light blue background.</p>\n"
    "<button onclick=\"window.location.href = 'https://www.youtube.com/';\">Visit YouTube</button>\n"
    "</body>\n"
    "</html>";*/

    ServerConfig() : port(PORT), welcomeMessage("Hello, World from Partie B!\n") {}
};

void main_C(int client_socket);

//  http://localhost:8080
//  g++ -o server test.cpp
//  ./server 