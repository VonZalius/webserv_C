#include "webserv.hpp"

/*std::string readFileContent(std::string& filePath)
{
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open())
    {
        std::cerr << "Impossible d'ouvrir le fichier : " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << fileStream.rdbuf();

    return buffer.str();
}*/

// Fonction pour initialiser et lancer le serveur
void startServer(s_server& config, int test_mode)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Création du socket du serveur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Attacher le socket au port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config.port);
    
    // Force le port 8080 à attacher au socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is running on port " << config.port << std::endl;

    while(true)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Traitement de la requête avec la Partie C
        std::cout << std::endl << "-------------------- Partie C --------------------" << std::endl << std::endl;
        Part_C part_c(new_socket, config, test_mode);

        close(new_socket);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "enter test mode value\n";
        return 0;
    }
    s_server config; // Utiliserait normalement les données provenant de la Partie A

    //init test
    config.server_name = "webserv";
    config.port = 8080;
    config.client_max_body_size = 9999;

    config.error_pages[404] = "webpage/error_pages/error404.html";
    config.error_pages[405] = "webpage/error_pages/error405.html";

    std::map<std::string, std::string> details1;
    details1["index"] = "index.html";
    details1["methods"] = "GET POST";
    details1["root"] = "/index.html";
    config.routes["/"] = details1;

    std::map<std::string, std::string> details2;
    details2["methods"] = "GET";
    details2["redirection"] = "https://signin.intra.42.fr";
    config.routes["intra42"] = details2;


    startServer(config, atoi(argv[1]));
    return 0;
}
