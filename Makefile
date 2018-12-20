.PHONY = all clean

CXX=g++
CXXFLAGS=-o -Wall -Wextra -pedantic -Os
UTILS_LIB=-ljsoncpp
UTILS_FILES= src/utils.cpp
USB_MON_LIB=-ludev
USB_MON_FILES= src/usb_monitor.cpp 
OBJ= utils.o usb_monitor.o

all: usb_monitor

utils.o:
	@echo "Compiling utils.o"
	@$(CXX) $(CXXFLAGS) -o utils.o -c $(UTILS_FILES)

usb_monitor.o: src/utils.h 
	@echo "Compiling usb_monitor.o"
	@$(CXX) $(CXXFLAGS) -o usb_monitor.o -c $(USB_MON_FILES)

usb_monitor: utils.o usb_monitor.o
	@echo "Compiling main.o"
	@$(CXX) $(CXXFLAGS) -time -o usb_monitor $(OBJ) src/main.cpp $(UTILS_LIB) $(USB_MON_LIB)


clean:
	@echo "Cleaning object files."
	@rm -f *.o
