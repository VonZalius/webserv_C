#include "webserv.hpp"

// Fonction pour initialiser et lancer le serveur
void startServer(const ServerConfig& config)
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
        main_C(new_socket, config);

        close(new_socket);
    }
}

int main()
{
    ServerConfig config; // Utiliserait normalement les données provenant de la Partie A
    startServer(config);
    return 0;
}
