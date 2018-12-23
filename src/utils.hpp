#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED
#include <cstdlib>
#include <arpa/inet.h>
#include <errno.h>
#include <json/writer.h>
#include <inttypes.h>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

namespace Utils
{
    const unsigned long int HALF_SEC_NANO = 500000000L;

    void err_exit(const std::string, unsigned int);
    void fatal(const char*, unsigned int);
    void wait();

    std::string serialize_data(const char*, const char*, const char*, const char*);
    void send_data(const char*, int, std::string);

    /* Basic strtoumax wrapper */
    uintmax_t strtoumax_(const char*);
    void signal_handler(int);
    void sigaction_(int);

    class Socket
    {
    private:
        int sockfd;
        const char* server_ip;
        int server_port;
        struct sockaddr_in server_addr;

    public:
        Socket(const char* s_ip, int s_port) : server_ip(s_ip), server_port(s_port) {}
        void start();
        void send_(std::string);
        void stop();
        virtual ~Socket() { stop(); }
    };
}
#endif // UTILS_H_INCLUDED
