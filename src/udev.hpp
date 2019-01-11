#ifndef UDEV_HPP
#define UDEV_HPP
#include <iostream>
#include <libudev.h>
#include <memory>
#include <string>

typedef struct udev* udev_ptr;
typedef struct udev_monitor* udev_mon_ptr;
typedef struct udev_device* udev_device_ptr;

class Udev
{
    private:
        std::unique_ptr<udev_ptr> udev_obj;
        std::unique_ptr<udev_mon_ptr> udev_mon;

    public:
        Udev();
        int init_monitor();
        std::unique_ptr<udev_device_ptr> get_udev_device();
        virtual ~Udev();
};

class UdevException : public std::exception
{
    private:
        const std::string msg;

    public:
        UdevException(const std::string &m) : msg(m) {}

        const char* what() const noexcept
        {
            return msg.c_str();
        }

        virtual ~UdevException();
};

#endif // UDEV_HPP
