#include "../webserv.hpp"

class HttpRequest
{
public:
    std::string method;
    std::string uri;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;

    HttpRequest();

    void parse(const std::string& requestText);
    void print_parse();
    void method_GET();
    void method_POST();
    void method_DELETE();

private:
};