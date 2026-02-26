#include <exception>
#include <string>

class outOfBoundsException : public std::exception{
    std::string errorMsg;
    public:
    outOfBoundsException(std::string message) : errorMsg(std::move(message)){}
    const char* what() const noexcept override {return errorMsg.c_str(); }
};
