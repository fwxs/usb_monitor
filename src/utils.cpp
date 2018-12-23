#include "utils.hpp"

static int sockfd;
static struct sockaddr_in server_addr;

void Utils::err_exit(const std::string msg, int status)
{
    std::cerr << "Error (" << status << "): " << msg << ". " << strerror(status) << std::endl;
    std::exit(status);
}

void Utils::fatal(const std::string msg, int status)
{
    perror(msg.c_str());
    std::exit(status);
}

void Utils::wait()
{
    std::this_thread::sleep_for(std::chrono::nanoseconds(Utils::HALF_SEC_NANO));
}

std::string Utils::serialize_data(const std::string action,
                                  const std::string dev_type,
                                  const std::string dev_path,
                                  const std::string dev_name)
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

void Utils::send_data(const std::string server_ip,
                      const unsigned short port,
                      const std::string msg)
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

    if(inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0){
        int errsv = errno;
        Utils::fatal("inet_pton", errsv);
    }

    /* Zero out unused structure variable */
    memset(&server_addr.sin_zero, 0, sizeof(server_addr));

    if(connect(sockfd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0){
        int errsv = errno;
        Utils::fatal("connect failed", errsv);
    }
}

void Utils::Socket::send_(const std::string msg)
{
    std::cout << "Sending " << msg.length() << " bytes of data." << std::endl;

    if(send(sockfd, msg.c_str(), msg.length(), 0) < 0){
        int errsv = errno;
        Utils::fatal("send failed", errsv);
    }
}

void Utils::Socket::stop()
{
    if(close(sockfd) < 0){
        int errsv = errno;
        Utils::fatal("close", errsv);
    }
}

uintmax_t Utils::strtoumax_(const char* str)
{
    uintmax_t ret = strtoumax(str, nullptr, 10);
    if(ret <= 0 || ( ret == UINTMAX_MAX && errno == ERANGE))
        Utils::err_exit("strtoumax failed", ERANGE);
    return ret;
}

void Utils::sigaction_(void (*func)(int))
{
    struct sigaction sa;

    memset(&sa, 0, sizeof (struct sigaction));

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if(sigaction(SIGINT, &sa, nullptr) == -1){
        int errsv = errno;
        Utils::fatal("sigaction", errsv);
    }
}
