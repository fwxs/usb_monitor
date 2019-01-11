#include "usb_monitor.hpp"

/*
 Code based on http://www.signal11.us/oss/udev/
*/
static std::unique_ptr<Udev> u;
static std::map<std::string, std::map<std::string, std::string>> devices;
static std::string server_ip;
static unsigned short server_port;

void UsbMonitor::init(const std::string &r_ip, const unsigned short r_port)
{
    server_ip = r_ip;
    server_port = r_port;

    // Initialize udev object.
    u = std::make_unique<Udev>();
}

void UsbMonitor::start()
{
    /* Monitor for usb connections */
    int fd = u->init_monitor();
    std::unique_ptr<udev_device_ptr> udev_device;

    std::cout << "[*] Waiting for usb devices." << '\n';

    while(1){
        fd_set fds;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        // Set tv.tv_sec & tv.tv_usec to 0.
        struct timeval tv = {0, 0};


        if((ret = select(fd + 1, &fds, nullptr, nullptr, &tv)) < 0){
            throw std::runtime_error(std::strerror(errno));
        }

        if(ret > 0 && FD_ISSET(fd, &fds)){
            if((udev_device = u->get_udev_device())){
                std::string action = udev_device_get_action(*udev_device);

                if(action == "add")
                    report_usb_connection(action, udev_device.get());
                else if(action == "remove")
                    report_usb_disconnection(action, udev_device.get());

            }
            udev_device_unref(*udev_device);
        }
        Utils::wait();
    }
}

void UsbMonitor::report_usb_connection(const std::string &action, udev_device_ptr* udev_device)
{
    std::cout << "[*] Device connected." << '\n';
    std::string sysname(udev_device_get_sysname(*udev_device));

    // Connected device data.
    devices[sysname].insert(std::make_pair("dev_path", udev_device_get_devpath(*udev_device)));
    devices[sysname].insert(std::make_pair("dev_manufacturer", udev_device_get_sysattr_value(*udev_device, "manufacturer")));
    devices[sysname].insert(std::make_pair("product", udev_device_get_sysattr_value(*udev_device, "product")));
    devices[sysname].insert(std::make_pair("serial", udev_device_get_sysattr_value(*udev_device, "serial")));
    devices[sysname].insert(std::make_pair("product_id", udev_device_get_sysattr_value(*udev_device, "idProduct")));
    devices[sysname].insert(std::make_pair("vendor_id", udev_device_get_sysattr_value(*udev_device, "idVendor")));
    devices[sysname].insert(std::make_pair("speed", udev_device_get_sysattr_value(*udev_device, "speed")));
    devices[sysname].insert(std::make_pair("max_pkt_size", udev_device_get_sysattr_value(*udev_device, "bMaxPacketSize0")));
    devices[sysname].insert(std::make_pair("max_power", udev_device_get_sysattr_value(*udev_device, "bMaxPower")));

    // Report device connection to remote server.
    Utils::send_data(server_ip, server_port, Utils::serialize_data(action, devices[sysname]));
}

void UsbMonitor::report_usb_disconnection(const std::string &action, udev_device_ptr* udev_device)
{
    std::cout << "[*] Device disconnected." << '\n';
    std::string sysname(udev_device_get_sysname(*udev_device));

    // Report device disconnection to remote server.
    Utils::send_data(server_ip, server_port, Utils::serialize_data(action, devices[sysname]));

    // Erase disconnected device record.
    devices.erase(sysname);
}

void UsbMonitor::signal_handler(int signum)
{
    std::cout << '\r' << "[!] Signal caught (" << signum << ")" << '\n';
    UsbMonitor::stop();
    std::exit(signum);
}

void UsbMonitor::stop()
{
    std::cout << "[*] Shutting down monitor." << '\n';
    u.reset(nullptr);

    std::cout << "[*] Flushing devices map." << '\n';
    devices.clear();
}
