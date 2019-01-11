#include "usb_monitor.hpp"


int main(int argc, char** argv)
{
    Utils::sigaction_(UsbMonitor::signal_handler);

    int exit_status = EXIT_SUCCESS;

    if(argc != 3){
        std::cout << "Usage: " << argv[0] << " <server ip> <port>" << std::endl;
        return exit_status;
    }

    try
    {
        unsigned short portno = static_cast<unsigned short>(Utils::strtoumax_(argv[2]));

        UsbMonitor::init(std::string(argv[1]), portno);
        UsbMonitor::start();
    }
    catch (UdevException &udev_err)
    {
        std::cerr << "UdevException: " << udev_err.what() << std::endl;
        exit_status = EXIT_FAILURE;
    }
    catch(Utils::SocketException &sock_err)
    {
        std::cerr << "SocketException: Error(" << sock_err.code() << ") " << sock_err.what() << std::endl;
        exit_status = sock_err.code();
    }
    catch(Utils::Exception &utils_err)
    {
        std::cerr << "UtilsException: Error(" << utils_err.code() << ") " << utils_err.what() << std::endl;
        exit_status = utils_err.code();
    }
    catch(std::exception &ex)
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        exit_status = EXIT_FAILURE;
    }

    return exit_status;
}
