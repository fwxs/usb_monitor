#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED
#include <arpa/inet.h>
#include <chrono>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <exception>
#include <inttypes.h>
#include <iostream>
#include <json/writer.h>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace Utils
{
    class Socket
    {
        private:
            const std::string _server_ip;
            const unsigned short _server_port;
            struct sockaddr_in _server_addr;
            int sockfd;

            void start();

        public:
            Socket(const std::string &s_ip, const unsigned short s_port) : _server_ip(s_ip),
                                                                           _server_port(s_port)
            {
                this->start();
            }

            const std::string &server_ip() const { return this->_server_ip; }
            const unsigned short &server_port() const { return this->_server_port; }

            void enable_keep_alive();
            bool establish_connection();
            void send_data(const std::string &);
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
    };

    /**
     * Half a second
     */
    const unsigned long int HALF_SEC_NANO = 500000000L;

    /**
     * Wait half a second.
    */
    void wait();
    void sigaction_(void (*func)(int));

    /**
     * Basic strtoumax wrapper.
     * @param str: String to convert to integer.
     * @return The integer representation of str.
    */
    uintmax_t strtoumax_(const char *str);

    /**
     * Serializes a map of strings into a JSON.
     *
     * @param action: Specifies if the action was add or remove device.
     * @param device_data: A map containing the USB data.
     *
     * @return serialized json.
     */
    std::string serialize_data(const std::string &action, const std::map<std::string, std::string> &device_data);

    bool isOnline(const std::string &rhost, const unsigned short port);
}
#endif // UTILS_H_INCLUDED
