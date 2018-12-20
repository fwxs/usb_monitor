#include "usb_monitor.h"

int main(int argc, char** argv)
{
    Utils::sigaction_(SIGINT);
    if(argc != 3){
        std::cout << "Usage: " << argv[0] << " <server ip> <port>" << std::endl;
        exit(1);
    }

    uintmax_t portno = Utils::strtoumax_(argv[2]);

    struct udev* _udev;
    if((_udev = udev_new()) == nullptr){
        std::cerr << "Error creating udev object" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::unique_ptr<UsbMonitor> usb_monitor(new UsbMonitor(_udev, argv[1], portno));
    usb_monitor->start();
    return 0;
}
