#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <arpa/inet.h>
#include <errno.h>
#include <json/writer.h>
#include <inttypes.h>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <time.h>
#include <unistd.h>

namespace Utils
{
    const unsigned long int HALF_SEC_NANO = 500000000L;

    void err_exit [[noreturn]] (const std::string, int);
    void fatal [[noreturn]] (const std::string, int);
    void wait();

    std::string serialize_data(const std::string, const std::string , const std::string , const std::string );
    void send_data(const std::string , const unsigned short, const std::string );

    /* Basic strtoumax wrapper */
    uintmax_t strtoumax_(const char*);
    void sigaction_(void (*func)(int));

    class Socket
    {
    private:
        const std::string server_ip;
        unsigned short server_port;

    public:
        Socket(const std::string s_ip, const unsigned short s_port) : server_ip(s_ip), server_port(s_port) {}
        void start();
        void send_(const std::string );
        void stop();
        ~Socket() { stop(); }
    };
}
#endif // UTILS_H_INCLUDED
