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

class UsbMonitor
{
private:
    struct udev* _udev;
    struct udev_device* u_dev;
    struct udev_monitor* mon;
    const char* server_ip;
    int server_port;
    int fd;       // File descriptor for the monitor.

    void report_usb_device();

public:
    UsbMonitor(struct udev* u, const char* s_ip, int s_p) : _udev(u), server_ip(s_ip), server_port(s_p) {}
    void start();
    void stop();
    virtual ~UsbMonitor() { stop(); }
};

#endif // USB_MONITOR_HPP_INCLUDED
