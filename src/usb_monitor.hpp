#ifndef USB_MONITOR_HPP_INCLUDED
#define USB_MONITOR_HPP_INCLUDED
#include "udev.hpp"
#include "utils.hpp"

#include <ctime>
#include <exception>
#include <iostream>
#include <libudev.h>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>


namespace  UsbMonitor
{

    void init(const std::string &, const unsigned short);
    void start [[noreturn]] ();
    void report_usb_connection(const std::string &, udev_device_ptr *);
    void report_usb_disconnection(const std::string &, udev_device_ptr *);
    void stop();
    void signal_handler [[noreturn]] (int);
}

#endif // USB_MONITOR_HPP_INCLUDED
