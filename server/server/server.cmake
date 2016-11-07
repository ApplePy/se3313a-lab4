include(${CMAKE_CURRENT_LIST_DIR}/../lib/lib.cmake)

set(server_HEADERS  server/include/server.hpp)

set(server_SOURCES  server/src/server.cpp
                    server/src/main.cpp)

add_executable(server ${server_SOURCES} ${server_HEADERS})
target_link_libraries(server se3313)

install(TARGETS server RUNTIME DESTINATION bin)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

if(DEV_ASN)
  target_include_directories(server PUBLIC ${lib_INCLUDE_DIRS})
else()
  target_include_directories(server INTERFACE ${lib_INCLUDE_DIRS})
endif()