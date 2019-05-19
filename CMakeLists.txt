cmake_minimum_required(VERSION 2.8)

project(usb_monitor)
set(SOURCE_DIR "src")
set(SOURCES ${SOURCE_DIR}/main.cpp ${SOURCE_DIR}/udev.cpp ${SOURCE_DIR}/usb_monitor.cpp ${SOURCE_DIR}/utils.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ludev -ljsoncpp -pipe")
add_executable(${PROJECT_NAME} "${SOURCES}")
