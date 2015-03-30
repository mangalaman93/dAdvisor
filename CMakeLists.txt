cmake_minimum_required(VERSION 2.8)

set (NO_INST_PROGS node/master.cpp node/client.cpp node/server.cpp node/snort.cpp)

ENABLE_TESTING()

INCLUDE_DIRECTORIES(BEFORE ${evpath_BINARY_DIR} ${evpath_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} include/)
set(CMAKE_CXX_FLAGS "-std=c++11")

foreach (PROG ${NO_INST_PROGS})
    ADD_EXECUTABLE(${PROG} ${PROG}.cpp src/utils.cpp src/guest.cpp src/lprocess.cpp src/monitor.cpp)
    SET_TARGET_PROPERTIES(${PROG} PROPERTIES LINKER_LANGUAGE ${CXX_EXECUTABLE_LINKER_LANGUAGE})
    TARGET_LINK_LIBRARIES(${PROG} evpath ${BASE_LIBS})
endforeach()
