#include "utils.hpp"


void Utils::err_exit(const std::string msg, unsigned int status)
{
    std::cerr << "Error (" << status << "): " << msg << std::endl;
    exit(status);
}

void Utils::fatal(const char* msg, unsigned int status)
{
    perror(msg);
    exit(status);
}

void Utils::wait()
{
    struct timespec req = {0, Utils::HALF_SEC_NANO}, rem = {0, 0};

    if(nanosleep(&req, &rem) < 0){
        int errsv = errno;
        fatal("nanosleep", errsv);
    }
}

std::string Utils::serialize_data(const char* action,
                                  const char* dev_type,
                                  const char* dev_path,
                                  const char* dev_name)
{
    Json::Value json_str;
    Json::StreamWriterBuilder stream_builder;

    json_str["Action"] = action;
    json_str["dev_type"] = dev_type;
    json_str["dev_path"] = dev_path;
    json_str["dev_name"] = dev_name;

    stream_builder["indentation"] = "";
    stream_builder["commentStyle"] = "None";
    return Json::writeString(stream_builder, json_str);
}

void Utils::send_data(const char* server_ip, int port, std::string msg)
{
    std::unique_ptr<Utils::Socket> send_sock(new Utils::Socket(server_ip, port));
    send_sock->start();
    send_sock->send_(msg);
    send_sock.reset();
}

void Utils::Socket::start()
{
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        int errsv = errno;
        Utils::fatal("socket failed", errsv);
    }

    /* Zero out server_addr memory */
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if(inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0){
        int errsv = errno;
        Utils::fatal("inet_pton", errsv);
    }

    /* Zero out unused structure variable */
    memset(&server_addr.sin_zero, 0, sizeof(server_addr));

    if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        int errsv = errno;
        fatal("connect failed", errsv);
    }
}

void Utils::Socket::send_(std::string msg)
{
    std::cout << "Sending " << msg.length() << " bytes of data." << std::endl;

    if(send(sockfd, msg.c_str(), msg.length(), 0) < 0){
        int errsv = errno;
        fatal("send failed", errsv);
    }
}

void Utils::Socket::stop()
{
    if(close(sockfd) < 0){
        int errsv = errno;
        fatal("close", errsv);
    }
}

uintmax_t Utils::strtoumax_(const char* str)
{
    uintmax_t ret = strtoumax(str, nullptr, 10);
    if(ret == 0 || ( ret == UINTMAX_MAX && errno == ERANGE))
        err_exit("strtoumax failed", ERANGE);

    return ret;

}


void Utils::signal_handler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ")" << std::endl;
    exit(signum);
}

void Utils::sigaction_(int signal_num)
{
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if(sigaction(signal_num, &sa, NULL) == -1){
        int errsv = errno;
        Utils::fatal("sigaction", errsv);
    }
}
