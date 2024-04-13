#ifndef PC_1_HPP
#define PC_1_HPP

#include "../webserv.hpp"

struct s_server;

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

    std::string cgi_content;

    //std::string delete_file_name;

    int status;
    std::map<int, std::string> _statusCodes;

    Part_C(int client_socket, s_server& config, int test_mode);

    void final_status();
    void init();
    void parse(const std::string& requestText, s_server& config);
    std::string getMultiPartBoundary();
    std::unordered_map<std::string, std::string>  parseMultiPartBody(const std::string &bodyLines);
    std::map<std::string, std::string> parseUrlEncoded(const std::string& data);
    void print_parse();
    void method_GET();
    void method_POST();
    void method_DELETE();
    void getContentType(const std::string& filePath);

    bool isCGI();
    void execute_cgi();

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