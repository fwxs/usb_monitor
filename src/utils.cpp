#include "utils.hpp"

static int sockfd;
static struct sockaddr_in server_addr;

void Utils::wait()
{
    std::this_thread::sleep_for(std::chrono::nanoseconds(Utils::HALF_SEC_NANO));
}

std::string Utils::serialize_data(const std::string &action,
                                  const std::map<std::string, std::string> &device_data)
{
    Json::Value json_str;
    Json::StreamWriterBuilder stream_builder;

    json_str["Action"] = action;

    // Pass the values from the device_data map to json_str.
    for(std::pair<std::string, std::string> element : device_data){
        json_str[element.first] = element.second;
    }

    stream_builder["indentation"] = "";
    stream_builder["commentStyle"] = "None";
    return Json::writeString(stream_builder, json_str);
}

void Utils::send_data(const std::string &server_ip, const unsigned short port, const std::string &msg)
{
    auto send_sock = std::make_unique<Utils::Socket>(server_ip, port);
    send_sock->start();
    send_sock->send_(msg);
}

void Utils::Socket::start()
{
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        int errsv = errno;
         throw Utils::SocketException(std::strerror(errsv), errsv);
    }

    /* Zero out server_addr memory */
    std::memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if(inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0){
        int errsv = errno;
        throw Utils::SocketException(std::strerror(errsv), errsv);
    }

    /* Zero out unused structure variable */
    std::memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    if(connect(sockfd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0){
        int errsv = errno;
        throw Utils::SocketException(std::strerror(errsv), errsv);
    }
}

void Utils::Socket::send_(const std::string &msg)
{
    if(send(sockfd, msg.c_str(), msg.length(), 0) < 0){
        int errsv = errno;
        throw Utils::SocketException(std::strerror(errsv), errsv);
    }
}

void Utils::Socket::stop()
{
    if(close(sockfd) < 0){
        int errsv = errno;
        throw Utils::SocketException(std::strerror(errsv), errsv);
    }
}

uintmax_t Utils::strtoumax_(const char* str)
{
    uintmax_t ret = strtoumax(str, nullptr, 10);
    if(ret <= 0 || ( ret == UINTMAX_MAX && errno == ERANGE)){
        int errsv = errno;
        throw Utils::Exception(std::strerror(errsv), errsv);
    }
    return ret;
}

void Utils::sigaction_(void (*func)(int))
{
    struct sigaction sa;

    std::memset(&sa, 0, sizeof (struct sigaction));

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if(sigaction(SIGINT, &sa, nullptr) == -1){
        int errsv = errno;
        throw Utils::SocketException(std::strerror(errsv), errsv);
    }
}

Utils::SocketException::~SocketException(){}
Utils::Exception::~Exception(){}
