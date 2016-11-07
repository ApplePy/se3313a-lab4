
find_package(Threads REQUIRED)
find_package(Boost 1.58 REQUIRED) # No libs required

include_directories(SYSTEM ${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

set(system_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

if(UNIX AND NOT APPLE)
    # Add librt if on Linux 
    list(APPEND system_LIBRARIES rt)
endif()