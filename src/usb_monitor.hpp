#ifndef USB_MONITOR_HPP_INCLUDED
#define USB_MONITOR_HPP_INCLUDED
/* Headers */
#include <iostream>
#include <libudev.h>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include "utils.hpp"

namespace  UsbMonitor
{

    void init(struct udev*, const std::string, const unsigned short) ;
    void start [[noreturn]] ();
    void report_usb_device(const std::string);
    void stop();
    void signal_handler [[noreturn]] (int);
}

#endif // USB_MONITOR_HPP_INCLUDED
