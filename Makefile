.PHONY = all clean

CXX=g++
WARNING_FLAGS=-Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wunused -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2 -Weffc++
OPTIMZ_FLAGS= -O2 -pipe -
CXXFLAGS=$(WARNING_FLAGS) $(OPTIMIZ_FLAGS)
UTILS_LIB=-ljsoncpp
UTILS_FILES= src/utils.cpp
USB_MON_LIB=-ludev
USB_MON_FILES= src/usb_monitor.cpp 
OBJ= utils.o usb_monitor.o

all: usb_monitor

utils.o:
	@echo "Compiling utils.o"
	@$(CXX) $(CXXFLAGS) -o utils.o -c $(UTILS_FILES)

usb_monitor.o: src/utils.hpp 
	@echo "Compiling usb_monitor.o"
	@$(CXX) $(CXXFLAGS) -o usb_monitor.o -c $(USB_MON_FILES)

usb_monitor: utils.o usb_monitor.o
	@echo "Compiling main.o"
	@$(CXX) $(CXXFLAGS) -time -o usb_monitor $(OBJ) src/main.cpp $(UTILS_LIB) $(USB_MON_LIB)


clean:
	@echo "Cleaning object files."
	@rm -f *.o
