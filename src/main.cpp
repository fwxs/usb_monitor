#include "usb_monitor.hpp"


int main(int argc, char** argv)
{
    Utils::sigaction_(UsbMonitor::signal_handler);

    if(argc != 3){
        std::cout << "Usage: " << argv[0] << " <server ip> <port>" << std::endl;
        exit(1);
    }

    unsigned short portno = static_cast<unsigned short>(Utils::strtoumax_(argv[2]));

    struct udev* u_obj;
    if((u_obj = udev_new()) == nullptr){
        std::cerr << "Error creating udev object" << std::endl;
        exit(EXIT_FAILURE);
    }
    UsbMonitor::init(u_obj, std::string(argv[1]), portno);
    UsbMonitor::start();
    return 0;
}
