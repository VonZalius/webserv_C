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


    if (!fileStream.is_open())
        status = 404;

    if (status == 404)
    {
        status = 404;
        contentType = "text/plain";
        content = "Page not found... very sorry :(";
    }
    /*else
    {
        getContentType(filePath);

        std::stringstream response_buffer;
        response_buffer << fileStream.rdbuf();
        content = response_buffer.str();
    }*/


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

bool areAllPathCharactersValid(const std::string &path)
{
	const std::string allowedSpecialChars = "_-.~:/?#[]@!$&'()*+,;=";
	for (size_t i = 0; i < path.length(); i++)
	{
		if (!isalnum(path[i]) && allowedSpecialChars.find(path[i]) == std::string::npos)
			return false;
	}
	return true;
}

bool isSafePath(const std::string &path)
{
	// Check for directory traversal
	return path.find("..") == std::string::npos;
}

bool isPathLengthValid(const std::string &path, size_t maxLength)
{
	return path.length() <= maxLength;
}

void Part_C::parse(const std::string& requestText, ServerConfig& config)
{
    std::istringstream requestStream(requestText);
    std::string line;

    // Parse de la ligne de requête
    if (std::getline(requestStream, line)) {
        std::istringstream lineStream(line);
        lineStream >> method >> uri >> httpVersion;
    }
    if (method != "GET" && method != "POST" && method != "DELETE")
	{
		status = 501;
		throw Part_C::InvalidRequestException("Error Method 501");
	}
    if (!areAllPathCharactersValid(uri))
	{
		status = 400;
		throw Part_C::InvalidRequestException("Error Uri 400");
	}
	if (!isSafePath(uri))
	{
		status = 403;
		throw Part_C::InvalidRequestException("Error Uri 403");
	}
	if (!isPathLengthValid(uri, 4096))
	{
		status = 414;
		throw Part_C::InvalidRequestException("Error Uri 414");
	}
    if (httpVersion != "HTTP/1.1" && httpVersion != "HTTP/1.0")
	{
		status = 505;
		throw Part_C::InvalidRequestException("Error HttpVersion 505");
	}

    // Parse des en-têtes
    while (std::getline(requestStream, line))
    {
        if (line.empty()) break; // Vérifie la fin des en-têtes grâce à la ligne vide
        auto colonPos = line.find(":");
        if (colonPos != std::string::npos && colonPos != 0)
        { // Vérifie aussi que ':' n'est pas le premier caractère
            std::string headerName = line.substr(0, colonPos);
            // Assurez-vous qu'il y a un espace après ':' avant de commencer la valeur
            std::string headerValue = (colonPos + 2 < line.size()) ? line.substr(colonPos + 2) : "";

            // Trim leading and trailing whitespaces from headerName and headerValue if necessary
            // ...

            if (!headerName.empty() && !headerValue.empty())
            { // Vérifie que ni le nom ni la valeur de l'en-tête ne sont vides
                headers[headerName] = headerValue; // Ajout de l'en-tête à la map
            }
            else
            {
                // Gestion d'erreur ou log si le nom de l'en-tête ou la valeur est vide
                status = 400;
		        throw Part_C::InvalidRequestException("Error Headers 400");
            }
        }
        else
        {
            // Gestion d'erreur ou log si la ligne d'en-tête est mal formée (pas de ':' ou ':' est le premier caractère)
            status = 400;
		    throw Part_C::InvalidRequestException("Error Headers 400");
        }
    }

    // Le reste est le corps de la requête
    std::string potential_body = std::string(std::istreambuf_iterator<char>(requestStream), {});
    if (potential_body.size() > config.max_body_size)
    {
        status = 413;
        throw Part_C::InvalidRequestException("Error Body 413");
    }
    if (headers.find("Content-Type") != headers.end())
    {
        if(headers["Content-Type"].find("multipart/form-data") != std::string::npos)
        {
            std::cout << "\n-----> Body form multipart/form-data\n\n";
        }
        else if(headers["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos)
        {
            std::cout << "\n-----> Body form application/x-www-form-urlencoded\n\n";
            post_file_name = "test";
            post_file_content = "contenu de test";
        }
        else
        {
            std::cout << "\n-----> Body form not supported\n\n";
        }
    }
    std::string body = potential_body;
}

void Part_C::print_parse()
{
    std::cout << "-------------------> Impression du parsing\n";
    std::cout << "method : " << method << std::endl;
    std::cout << "uri : " << uri << std::endl;
    std::cout << "httpVersion : " << httpVersion << std::endl;
    std::cout << "headers : " << std::endl;
    for (const auto& pair : headers)
    {
        std::cout << "     "<< pair.first << " : " << pair.second << std::endl;
    }
    std::cout << "body : " << body << std::endl;
    std::cout << std::endl;
}

void Part_C::method_GET()
{
    std::cout << std::endl << "-------------------> GET" << std::endl;
}

std::unordered_map<std::string, std::string> parseUrlEncodedData(const std::string& data)
{
    std::unordered_map<std::string, std::string> result;
    std::istringstream dataStream(data);
    std::string pair;

    while (std::getline(dataStream, pair, '&')) {
        size_t equalPos = pair.find('=');
        if (equalPos != std::string::npos) {
            std::string key = pair.substr(0, equalPos);
            std::string value = pair.substr(equalPos + 1);
            result[key] = value; // Dans une version complète, vous devriez décoder les pourcentages ici.
        }
    }

    return result;
}

bool checkFileExists(const std::string &path)
{
	std::ifstream file(path.c_str());
	return (file.good());
}

void Part_C::method_POST()
{
    std::cout << std::endl << "-------------------> POST" << std::endl;

    std::string upload_path = "./the_ultimate_webserv/post/" + post_file_name;
	if (checkFileExists(upload_path))
	{
		status= 409; // Conflict
		throw Part_C::InvalidRequestException("Error Post 409");
	}

	std::ofstream file;
	file.open(upload_path, std::ios::out);

	if (!file.is_open())
	{
		status = 500; // Internal Server Error
		throw Part_C::InvalidRequestException("Error Post 500");
	}
	file << post_file_content;
	file.close();

	status = 201; // Created
    contentType = "text/plain"; // Ajustez selon le type de réponse que vous voulez renvoyer
    content = "Resource created successfully."; // Personnalisez le message selon le résultat du traitement

    /*// Ici, vous pouvez ajouter une logique pour traiter différents types de contenu
    if (headers["Content-Type"] == "application/x-www-form-urlencoded")
    {
        auto postData = parseUrlEncodedData(body);

        // Afficher les données postData
        std::cout << std::endl << "---> Is application/x-www-form-urlencoded" << std::endl;
        for (const auto& pair : postData) {
            std::cout << "Clé: " << pair.first << ", Valeur: " << pair.second << std::endl;
        }
        
        // Traitez les données de postData selon vos besoins spécifiques
        // Par exemple, valider les données, les stocker dans une base de données, etc.
    }
    else if (headers["Content-Type"] == "application/json")
    {
        std::cout << std::endl << "---> Is application/json" << std::endl;
        // Si vous attendez du JSON, vous devrez le parser ici
        // Assurez-vous d'inclure une bibliothèque JSON pour cette tâche
    }

    // Après avoir traité les données, configurez la réponse
    // Supposons que le traitement a été réussi et que vous souhaitez retourner un 200 OK
    // ou un 201 Created si vous avez créé une ressource
    status = 201; // Par exemple, pour "Created"
    contentType = "text/plain"; // Ajustez selon le type de réponse que vous voulez renvoyer
    content = "Resource created successfully."; // Personnalisez le message selon le résultat du traitement*/
}

void Part_C::method_DELETE()
{
    std::cout << std::endl << "-------------------> DELETE" << std::endl;
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

