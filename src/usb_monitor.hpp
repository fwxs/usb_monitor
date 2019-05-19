#ifndef USB_MONITOR_HPP_INCLUDED
#define USB_MONITOR_HPP_INCLUDED
#include "udev.hpp"
#include "utils.hpp"

#include <algorithm>
#include <ctime>
#include <libudev.h>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <utility>


namespace UsbMonitor
{
    void start [[noreturn]] (const std::string &rhost, const unsigned short rport);
    void report_usb_connection(const std::string &, udev_device_ptr *);
    void report_usb_disconnection(const std::string &, udev_device_ptr *);
    void stop();
    void signal_handler [[noreturn]] (int);

    class UsbInfo
    {
        private:
            std::map<std::string, std::string> _properties;

        public:
            UsbInfo(const udev_device_ptr* udev_device);

            const std::map<std::string, std::string> &properties() const { return _properties; }

            virtual ~UsbInfo();
    };
}

#endif // USB_MONITOR_HPP_INCLUDED
