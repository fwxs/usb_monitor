#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED
#include <arpa/inet.h>
#include <chrono>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <exception>
#include <inttypes.h>
#include <json/writer.h>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace Utils
{
    const unsigned long int HALF_SEC_NANO = 500000000L;
    void wait();

    std::string serialize_data(const std::string &, const std::map<std::string, std::string> &);
    void send_data(const std::string &, const unsigned short, const std::string &);

    /* Basic strtoumax wrapper */
    uintmax_t strtoumax_(const char*);
    void sigaction_(void (*func)(int));

    class Socket
    {
    private:
        const std::string server_ip;
        unsigned short server_port;

    public:
        Socket(const std::string &s_ip, const unsigned short s_port) : server_ip(s_ip), server_port(s_port) {}
        void start();
        void send_(const std::string &);
        void stop();
        ~Socket() { stop(); }
    };

    class SocketException : public std::exception
    {
        private:
            const std::string msg;
            int err_code;

        public:
            SocketException(const std::string &m, int e_c) : msg(m), err_code(e_c) {}
            const char* what() const noexcept
            {
                return msg.c_str();
            }

            int code() const noexcept
            {
                return err_code;
            }

            virtual ~SocketException();
    };

    class Exception : public std::exception
    {
    private:
        const std::string msg;
        int err_code;

    public:
        Exception(const std::string& m, const int c) : msg(m), err_code(c) {}
        const char* what() const noexcept
        {
            return msg.c_str();
        }

        int code() const noexcept
        {
            return err_code;
        }

        virtual ~Exception();
    };
}
#endif // UTILS_H_INCLUDED
