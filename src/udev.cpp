#include "udev.hpp"

Udev::Udev()
{
    udev_obj = std::make_unique<udev_ptr>(udev_new());

    if(*udev_obj == nullptr)
        throw UdevException("Error allocating a new udev pointer");
}

int Udev::init_monitor()
{
    udev_mon = std::make_unique<udev_mon_ptr>(udev_monitor_new_from_netlink(*udev_obj, "udev"));

    if(*udev_mon == nullptr)
        throw UdevException("Error allocating udev monitor pointer");

    if(udev_monitor_filter_add_match_subsystem_devtype(*udev_mon, "usb", "usb_device") < 0)
        throw UdevException("Error setting usb device filtering");

    if(udev_monitor_enable_receiving(*udev_mon) < 0)
        throw UdevException("Error setting up devices monitoring.");

    int mon_fd = udev_monitor_get_fd(*udev_mon);

    if(mon_fd < 0)
        throw UdevException("Error getting monitor file descriptor");

    return mon_fd;
}

std::unique_ptr<udev_device_ptr> Udev::get_udev_device()
{
    return std::make_unique<udev_device_ptr>(udev_monitor_receive_device(*udev_mon));
}

Udev::~Udev()
{
    // Clear udev monitor.
    if(*udev_mon != nullptr)
    {
        udev_monitor_unref(*udev_mon);
        udev_mon.reset(nullptr);
    }

    // Clean udev obj.
    if(*udev_obj != nullptr)
    {
        udev_unref(*udev_obj);
        udev_obj.reset(nullptr);
    }
}

UdevException::~UdevException(){}
