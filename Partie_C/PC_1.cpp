#include "PC_1.hpp"

// Fonction pour traiter une requête HTTP reçue et renvoyer une réponse
Part_C::Part_C(int client_socket, ServerConfig& config, int test_mode): test_mode(test_mode)
{

    init();
    status = 200;

    //-------------------- Partie Request --------------------

    const int bufferSize = 1024;
    char request_buffer[bufferSize] = {0};

    // Lecture de la requête du client
    if (test_mode == false)
    {
        int bytesReceived = read(client_socket, request_buffer, bufferSize - 1);
        if (bytesReceived < 1)
        {
            std::cout << "Erreur de lecture ou connexion fermée par le client." << std::endl;
            return;
        }
    }

    //TEST
    if (test_mode == true)
    {
        int fd = open("test.txt", O_RDONLY);
        off_t fileSize = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        read(fd, request_buffer, fileSize);
    }


    std::cout << std::endl << std::endl << "-------------------> Request" << std::endl << request_buffer << std::endl;

    try
    {
        parse(request_buffer, config);
        print_parse();

    //-------------------- Partie Execution --------------------

        if(method == "GET")
            method_GET();
        else if(method == "POST")
            method_POST();
        else if(method == "DELETE")
            method_DELETE();

    }

    catch(InvalidRequestException &e)
    {
        std::cout << "-----> Error ! It seem to be a : " << status << '\n';
    }

    //-------------------- Partie Response --------------------

     // Utilisez basePath de la configuration pour trouver les fichiers
    std::string requestURI = uri == "/" ? config.index : uri;
    std::string filePath = config.basePath + requestURI;
    std::ifstream fileStream(filePath);
    std::string httpResponse;


    if (method == "GET")
    {
        if (!fileStream.is_open())
            status = 404;

        getContentType(filePath);

        std::stringstream response_buffer;
        response_buffer << fileStream.rdbuf();
        content = response_buffer.str();
    }

    final_status();

    httpResponse = "HTTP/1.1 " + std::to_string(status) + " " + _statusCodes[status] + "\r\n" +
               "Content-Type: " + contentType + "\r\n" +
               "Content-Length: " + std::to_string(content.length()) + "\r\n" +
               //"Connection: close\r\n" +
               "\r\n" +
               content;

    std::cout << std::endl << std::endl << "-------------------> Response" << std::endl << httpResponse << std::endl;

    // Envoi de la réponse au client
    send(client_socket, httpResponse.c_str(), httpResponse.length(), 0);

}

void Part_C::final_status()
{
    if (status != 200 && status != 201)
    {
        contentType = "text/plain";
        content = std::to_string(status) + " : " + _statusCodes[status];
    }
}

void Part_C::init()
{
    _statusCodes[200] = "OK";
	_statusCodes[201] = "Created";
	_statusCodes[202] = "Accepted";
	_statusCodes[204] = "No Content";
	_statusCodes[301] = "Moved Permanently";
	_statusCodes[302] = "Found";
	_statusCodes[304] = "Not Modified";
	_statusCodes[400] = "Bad Request";
	_statusCodes[401] = "Unauthorized";
	_statusCodes[403] = "Forbidden";
	_statusCodes[404] = "Not Found";
	_statusCodes[405] = "Method Not Allowed";
	_statusCodes[409] = "Conflict";
	_statusCodes[413] = "Payload Too Large";
	_statusCodes[500] = "Internal Server Error";
	_statusCodes[501] = "Not Implemented";
	_statusCodes[505] = "HTTP Version Not Supported";
	_statusCodes[508] = "Infinite Loop Detected";
}

void Part_C::getContentType(const std::string& filePath)
{
    if (filePath.rfind(".html") == filePath.length() - 5)
        contentType = "text/html";
    else if (filePath.rfind(".css") == filePath.length() - 4) 
        contentType = "text/css";
    else if (filePath.rfind(".js") == filePath.length() - 3) 
        contentType = "application/javascript";
    else if (filePath.rfind(".png") == filePath.length() - 4) 
        contentType = "image/png";
    else if (filePath.rfind(".jpg") == filePath.length() - 4 || filePath.rfind(".jpeg") == filePath.length() - 5) 
        contentType = "image/jpeg";
    else
        contentType = "text/plain"; // Type par défaut
}

