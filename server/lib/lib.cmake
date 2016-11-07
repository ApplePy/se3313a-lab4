
if(NOT TARGET se3313)
    include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)

    set(lib_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include)

    set(lib_INCLUDES    
                        lib/include/msg/instance.hpp
                        lib/include/msg/error.hpp
                        lib/include/msg/login.hpp
                        lib/include/msg/message.hpp

                        lib/include/msg/visitor.hpp

                        lib/include/msg/json.hpp

                        lib/include/networking/flex_waiter.hpp
                        lib/include/networking/socket.hpp
                        lib/include/networking/socket_server.hpp)

    set(lib_SOURCES     lib/src/msg/instance.cpp
    
                        lib/src/msg/error.cpp
                        lib/src/msg/login.cpp
                        lib/src/msg/message.cpp

                        lib/src/networking/flex_waiter.cpp
                        lib/src/networking/socket.cpp
                        lib/src/networking/socket_server.cpp)

    add_library(se3313 ${lib_SOURCES} ${lib_INCLUDES})
    target_link_libraries(se3313 ${system_LIBRARIES})
    target_include_directories(se3313 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

    install(TARGETS se3313 ARCHIVE DESTINATION lib/)
endif()