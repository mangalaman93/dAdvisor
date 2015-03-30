cmake_minimum_required(VERSION 2.8)

set (NO_INST_PROGS master client)

ENABLE_TESTING()

INCLUDE_DIRECTORIES(BEFORE ${evpath_BINARY_DIR} ${evpath_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} include/)
set(CMAKE_CXX_FLAGS "-std=c++11")

foreach (PROG ${NO_INST_PROGS})
    ADD_EXECUTABLE(node_${PROG} node/${PROG}.cpp src/utils.cpp src/guest.cpp src/lprocess.cpp src/monitor.cpp)
    TARGET_LINK_LIBRARIES(node_${PROG} evpath ${BASE_LIBS})
endforeach()
