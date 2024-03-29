#ifndef PC_1_HPP
#define PC_1_HPP

#include "../webserv.hpp"

struct ServerConfig;

class Part_C
{
public:

    bool test_mode = false;

    std::string method;
    std::string uri;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string contentType;
    std::string content;

    std::string post_file_name;
    std::string post_file_content;

    int status;
    std::map<int, std::string> _statusCodes;

    Part_C(int client_socket, ServerConfig& config, int test_mode);

    void init();
    void parse(const std::string& requestText, ServerConfig& config);
    std::string getMultiPartBoundary();
    std::unordered_map<std::string, std::string>  parseMultiPartBody(const std::string &bodyLines);
    void print_parse();
    void method_GET();
    void method_POST();
    void method_DELETE();
    void getContentType(const std::string& filePath);

    class InvalidRequestException : public std::exception
	{
	public:
		InvalidRequestException(const std::string &message) : _message(message) {}
		virtual const char *what(void) const throw()
		{
			return (_message.c_str());
		}
		virtual ~InvalidRequestException() throw() {}
		InvalidRequestException(void) : _message("Invalid request") {}

	private:
		std::string _message;
	};

private:
};

#endif