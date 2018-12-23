#include "usb_monitor.hpp"

/*
 Code based on http://www.signal11.us/oss/udev/
*/

void UsbMonitor::start()
{
    /* Monitor for usb connections */
    if((mon = udev_monitor_new_from_netlink(_udev, "udev")) == nullptr)
        Utils::err_exit("udev_monitor_new_from_netlink", EXIT_FAILURE);

    if(udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", "usb_device") < 0)
        Utils::err_exit("udev_monitor_filter_add_match_subsystem_devtype", EXIT_FAILURE);

    if(udev_monitor_enable_receiving(mon) < 0)
        Utils::err_exit("udev_monitor_enable_receiving", EXIT_FAILURE);

    if((fd = udev_monitor_get_fd(mon)) < 0)
        Utils::err_exit("udev_monitor_get_fd", EXIT_FAILURE);

    std::cout << "[*] Waiting for usb devices." << std::endl;

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
                const char* action = udev_device_get_action(u_dev);
                if(strncmp(action, "add", 3) == 0 || strncmp(action, "remove", 6) == 0)
                    report_usb_device();
            }
            udev_device_unref(u_dev);
        }
        Utils::wait();
    }
}

void UsbMonitor::report_usb_device()
{

    std::string json_string = Utils::serialize_data(udev_device_get_action(u_dev),
                                                    udev_device_get_devtype(u_dev),
                                                    udev_device_get_devpath(u_dev),
                                                    udev_device_get_sysname(u_dev));
    Utils::send_data(server_ip, server_port, json_string);
}

void UsbMonitor::stop()
{
    std::cout << "[*] Shutting down monitor." << std::endl;
    udev_unref(_udev);
}

