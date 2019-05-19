#include "utils.hpp"

static const int ON = 1;
static const int OFF = 0;

// Socket Section.
void Utils::Socket::start()
{
    std::cout << "[*] Establishing connection with [" + _server_ip + ":" << _server_port << "]" << '\n';
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        int errsv = errno;
         throw Utils::SocketException(std::strerror(errsv), errsv);
    }

    /* Zero out server_addr memory */
    std::memset(&_server_addr, 0, sizeof(struct sockaddr_in));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(_server_port);

    if(inet_pton(AF_INET, _server_ip.c_str(), &_server_addr.sin_addr) <= 0){
        int errsv = errno;
        throw Utils::SocketException(std::strerror(errsv), errsv);
    }

    /* Zero out unused structure variable */
    std::memset(&_server_addr.sin_zero, 0, sizeof(_server_addr.sin_zero));
}

void Utils::Socket::enable_keep_alive()
{
    if(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &ON, sizeof (ON)) == -1){
        std::cerr << "[!] Couldn't set KeepAlive option." << '\n';
        throw SocketException(std::strerror(errno), errno);
    }
}

bool Utils::Socket::establish_connection()
{
    if(connect(sockfd, reinterpret_cast<struct sockaddr *>(&_server_addr), sizeof(_server_addr)) < 0){
        int errsv = errno;
        if(errsv != EISCONN){
            std::cerr << "[*] Host is not alive! -> " << std::strerror(errno) << '\n';
            return false;
        }
    }
    return true;
}

void Utils::Socket::send_data(const std::string &msg)
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
// END Socket section.

// Utils Section.
void Utils::wait()
{
    std::this_thread::sleep_for(std::chrono::nanoseconds(Utils::HALF_SEC_NANO));
}

std::string Utils::serialize_data(const std::string &action, const std::map<std::string, std::string> &device_data)
{
    Json::Value json;
    Json::StreamWriterBuilder stream_builder;

    json["action"] = action;

    // Pass the values from the device_data map to json_str.
    for(std::pair<std::string, std::string> element : device_data){
        json[element.first] = element.second;
    }

    stream_builder["indentation"] = "";
    stream_builder["commentStyle"] = "None";

    return Json::writeString(stream_builder, json);
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
// END Utils Section
