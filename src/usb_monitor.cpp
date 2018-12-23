#include "usb_monitor.hpp"

/*
 Code based on http://www.signal11.us/oss/udev/
*/


static struct udev* udev_obj;
static struct udev_device* u_dev;
static struct udev_monitor* mon;
static std::string server_ip;
static unsigned short server_port;

void UsbMonitor::init(struct udev* u, const std::string s_ip, const unsigned short s_p)
{
    udev_obj = u;
    server_ip = s_ip;
    server_port = s_p;
}

void UsbMonitor::start()
{
    /* Monitor for usb connections */

    if((mon = udev_monitor_new_from_netlink(udev_obj, "udev")) == nullptr)
        Utils::err_exit("udev_monitor_new_from_netlink", EXIT_FAILURE);

    if(udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", "usb_device") < 0)
        Utils::err_exit("udev_monitor_filter_add_match_subsystem_devtype", EXIT_FAILURE);

    if(udev_monitor_enable_receiving(mon) < 0)
        Utils::err_exit("udev_monitor_enable_receiving", EXIT_FAILURE);

    int fd = udev_monitor_get_fd(mon);

    if(fd < 0)
        Utils::err_exit("udev_monitor_get_fd", EXIT_FAILURE);

    std::cout << "[*] Waiting for usb devices." << '\n';

    while(1){
        fd_set fds;
        struct timeval tv;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        if((ret = select(fd + 1, &fds, nullptr, nullptr, &tv)) < 0){
            int errsv = errno;
            Utils::fatal("select", errsv);
        }

        if(ret > 0 && FD_ISSET(fd, &fds)){
            if((u_dev = udev_monitor_receive_device(mon))){
                std::string action = udev_device_get_action(u_dev);
                if(action == "add" || action == "remove")
                    report_usb_device(action);
            }
            udev_device_unref(u_dev);
        }
        Utils::wait();
    }
}

void UsbMonitor::report_usb_device(const std::string action)
{

    std::string json_string = Utils::serialize_data(action,
                                                    std::string(udev_device_get_devtype(u_dev)),
                                                    std::string(udev_device_get_devpath(u_dev)),
                                                    std::string(udev_device_get_sysname(u_dev)));
    Utils::send_data(server_ip, server_port, json_string);
}

void UsbMonitor::signal_handler(int signum)
{
    std::cout << "Signal caught (" << signum << ")" << '\n';
    UsbMonitor::stop();
    std::exit(signum);
}

void UsbMonitor::stop()
{
    std::cout << "[*] Shutting down monitor." << std::endl;
    udev_monitor_unref(mon);
    udev_unref(udev_obj);
}
