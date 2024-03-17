#include "PC_1.hpp"

HttpRequest::HttpRequest()
{

}

void HttpRequest::parse(const std::string& requestText)
{
    std::istringstream requestStream(requestText);
    std::string line;
    
    // Parse de la ligne de requête
    if (std::getline(requestStream, line))
    {
        std::istringstream lineStream(line);
        lineStream >> method >> uri >> httpVersion;
    }
    
    // Parse des en-têtes
    while (std::getline(requestStream, line) && line != "\r")
    {
        auto colonPos = line.find(":");
        if (colonPos != std::string::npos)
        {
            std::string headerName = line.substr(0, colonPos);
            // +2 pour sauter ": ", -3 pour ignorer "\r"
            std::string headerValue = line.substr(colonPos + 2, line.size() - colonPos - 3); 
            headers[headerName] = headerValue;
        }
    }

    // Le reste est le corps
    body = std::string(std::istreambuf_iterator<char>(requestStream), {});
}

void HttpRequest::print_parse()
{
    std::cout << "-------------------> Impression du parsing\n";
    std::cout << "method : " << method << std::endl;
    std::cout << "uri : " << uri << std::endl;
    std::cout << "httpVersion : " << httpVersion << std::endl;
    std::cout << "headers : " << std::endl;
    for (const auto& pair : headers) {
        std::cout << "     "<< pair.first << " : " << pair.second << std::endl;
    }
    std::cout << "body : " << body << std::endl;
    std::cout << std::endl;
}

void HttpRequest::method_GET()
{
    std::cout << std::endl << "-------------------> GET" << std::endl<< std::endl;
}

void HttpRequest::method_POST()
{
    std::cout << std::endl << "-------------------> POST" << std::endl<< std::endl;
}

void HttpRequest::method_DELETE()
{
    std::cout << std::endl << "-------------------> DELETE" << std::endl<< std::endl;
}


// Fonction pour traiter une requête HTTP reçue et renvoyer une réponse
void main_C(int client_socket, const ServerConfig& config)
{

    //-------------------- Partie Request --------------------

    HttpRequest httpRequest;

    const int bufferSize = 1024;
    char buffer[bufferSize] = {0};

    // Lecture de la requête du client
    int bytesReceived = read(client_socket, buffer, bufferSize - 1);
    if (bytesReceived < 1)
    {
        std::cout << "Erreur de lecture ou connexion fermée par le client." << std::endl;
        return;
    }

    std::cout << std::endl << std::endl << "-------------------> Request" << std::endl << buffer << std::endl;

    httpRequest.parse(buffer);
    httpRequest.print_parse();

    //-------------------- Partie Execution --------------------

    if(httpRequest.method == "GET")
        httpRequest.method_GET();
    else if(httpRequest.method == "POST")
        httpRequest.method_POST();
    else if(httpRequest.method == "DELETE")
        httpRequest.method_DELETE();


    //-------------------- Partie Response --------------------

    std::string httpResponse;
    // Ici, vous pouvez décider de la réponse en fonction du parsing
    if (httpRequest.uri == "/")
    {
        httpResponse = config.httpResponse_blue; // Réponse personnalisée depuis config
    }
    else
    {
        httpResponse = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\n\nPage not found";
    }

    // Envoi de la réponse au client
    send(client_socket, httpResponse.c_str(), httpResponse.length(), 0);

}