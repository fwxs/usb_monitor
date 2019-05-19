#include "usb_monitor.hpp"

/*
 Code based on http://www.signal11.us/oss/udev/
*/
typedef class std::unique_ptr<UsbMonitor::UsbInfo> usb_info_ptr;

static std::vector<std::string> pending_devices;
static std::map<std::string, usb_info_ptr> devices;
static std::unique_ptr<Utils::Socket> socket_ptr;

static std::string server_ip;
static unsigned short server_port;

static std::string get_pending_devices(const std::string &last_device)
{
    std::string pending = "[";

    std::for_each(std::begin(pending_devices), std::end(pending_devices), [&] (std::string &dev_properties){
        pending.append(dev_properties + ',');
    });

    pending += last_device + "]";

    return pending;
}

static void report(const std::string& dev_properties){
    // Report device disconnection to remote server.
    if(socket_ptr->establish_connection())
    {
        if(pending_devices.empty())
        {
            socket_ptr->send_data(dev_properties);
        }
        else
        {
            socket_ptr->send_data(get_pending_devices(dev_properties));
            pending_devices.clear();
        }
    }
    else
    {
        socket_ptr = std::make_unique<Utils::Socket>(server_ip, server_port);
        socket_ptr->establish_connection();

        pending_devices.push_back(dev_properties);

        std::cout << "[*] Saving device info." << '\n';
        std::cout << "[+] " << pending_devices.size() << " devices on queue.\n";
    }
}

void UsbMonitor::start(const std::string &rhost, const unsigned short rport)
{
    server_ip = rhost;
    server_port = rport;

    socket_ptr = std::make_unique<Utils::Socket>(server_ip, server_port);
    socket_ptr->establish_connection();

    Udev udev = {};
    int fd = udev.init_monitor();

    std::unique_ptr<udev_device_ptr> udev_device;

    std::cout << "[*] Waiting for usb devices." << '\n';

    while(true){
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
            if((udev_device = udev.get_udev_device())){
                std::string action = udev_device_get_action(*udev_device);

                if(action == "add") {
                    report_usb_connection(action, udev_device.get());
                }
                else if(action == "remove"){
                    report_usb_disconnection(action, udev_device.get());
                }
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

    devices.emplace(sysname, std::make_unique<UsbMonitor::UsbInfo>(udev_device));
    std::string dev_properties(Utils::serialize_data(action, devices[sysname]->properties()));

    report(dev_properties);
}

void UsbMonitor::report_usb_disconnection(const std::string &action, udev_device_ptr* udev_device)
{
    std::cout << "[*] Device disconnected." << '\n';

    if(!devices.empty()){
        std::string sysname(udev_device_get_sysname(*udev_device));
        std::string dev_properties = Utils::serialize_data(action, devices[sysname]->properties());

        report(dev_properties);

        // Erase disconnected device record.
        devices.at(sysname).reset(nullptr);
        devices.erase(sysname);
    }
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
    std::cout << "[*] Flushing devices map." << '\n';
    devices.clear();
}

UsbMonitor::UsbInfo::UsbInfo(const udev_device_ptr* udev_device)
{
    this->_properties.emplace("dev_path", udev_device_get_devpath(*udev_device));

    this->_properties.emplace("manufacturer", udev_device_get_sysattr_value(*udev_device, "manufacturer"));
    this->_properties.emplace("product", udev_device_get_sysattr_value(*udev_device, "product"));

    this->_properties.emplace("serial", udev_device_get_sysattr_value(*udev_device, "serial"));
    this->_properties.emplace("product_id", udev_device_get_sysattr_value(*udev_device, "idProduct"));

    this->_properties.emplace("vendor_id", udev_device_get_sysattr_value(*udev_device, "idVendor"));
    this->_properties.emplace("speed", udev_device_get_sysattr_value(*udev_device, "speed"));

    this->_properties.emplace("max_speed", udev_device_get_sysattr_value(*udev_device, "bMaxPacketSize0"));
    this->_properties.emplace("power", udev_device_get_sysattr_value(*udev_device, "bMaxPower"));
}

UsbMonitor::UsbInfo::~UsbInfo()
{
    this->_properties.clear();
}
