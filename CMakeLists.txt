cmake_minimum_required(VERSION 3.5)

project(beats_server)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/proto
  ${PROJECT_SOURCE_DIR}/tool
)

# 将源文件列表保存到变量中
set(SOURCE_FILES
  mysocket.cc
  proto/project.pb.cc
  proto/server.pb.cc
  tool/circularbuffer.cc
  tool/pbhelper.cc
  serverbase.cc
)

add_executable(gateserver gateserver.cpp ${SOURCE_FILES})
add_executable(controlserver controlserver.cpp ${SOURCE_FILES})
add_executable(gameserver gameserver.cpp ${SOURCE_FILES})
add_executable(debugserver debugserver.cpp ${SOURCE_FILES})
add_executable(test test.cpp)

target_link_libraries(gateserver ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARY})
target_link_libraries(controlserver ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARY})
target_link_libraries(gameserver ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARY})
target_link_libraries(debugserver ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARY} -lprofiler -ljemalloc)
target_link_libraries(test ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARY} -lprofiler -ljemalloc)
