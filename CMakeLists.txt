cmake_minimum_required(VERSION 2.8)
PROJECT (boost_rx_tx)
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

find_package(OpenCV REQUIRED )


set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.71.0 REQUIRED COMPONENTS system)

if(Boost_FOUND)
include_directories(${Boost_INCLUDE_DIRS})

# add_executable( serial_pub serial_pub.cpp )
# target_link_libraries( serial_pub ${Boost_LIBRARIES} )

# add_executable( serial_sub serial_sub.cpp )
# target_link_libraries( serial_sub ${Boost_LIBRARIES} )

add_executable( serial_pub_simple serial_pub_simple.cpp )
target_link_libraries( serial_pub_simple ${Boost_LIBRARIES} )

# add_executable( send_img_tcp send_img_tcp.cpp )
# target_link_libraries( send_img_tcp ${Boost_LIBRARIES} )

# add_executable( recv_img_tcp recv_img_tcp.cpp )
# target_link_libraries( recv_img_tcp ${Boost_LIBRARIES} )

add_executable( client client.cpp )
target_link_libraries( client ${Boost_LIBRARIES} )

add_executable( server server.cpp )
target_link_libraries( server ${Boost_LIBRARIES} )

add_executable( img_send_simple img_send_simple.cpp )
target_link_libraries( img_send_simple ${Boost_LIBRARIES} )

add_executable( img_recv_simple img_recv_simple.cpp )
target_link_libraries( img_recv_simple ${Boost_LIBRARIES} )

# add_executable( serial_sub_simple serial_sub_simple.cpp )
# target_link_libraries( serial_sub_simple ${Boost_LIBRARIES} )

add_executable( tcp_cam_pub_client tcp_cam_pub_client.cpp )
target_link_libraries( tcp_cam_pub_client ${Boost_LIBRARIES} )

add_executable( tcp_cam_sub_server tcp_cam_sub_server.cpp )
target_link_libraries( tcp_cam_sub_server ${Boost_LIBRARIES} )

# add_executable( tcp_cam_client tcp_cam_client.cpp )
# target_link_libraries( tcp_cam_client ${Boost_LIBRARIES} )

# add_executable( tcp_cam_server tcp_cam_server.cpp )
# target_link_libraries( tcp_cam_server ${Boost_LIBRARIES} )

# add_executable( udp_server udp_server.cpp )
# target_link_libraries( udp_server ${Boost_LIBRARIES} )

# add_executable( udp_client udp_client.cpp umba_crc_table.c umba_crc_table.h)
# target_link_libraries( udp_client ${Boost_LIBRARIES} )

endif()

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

target_link_libraries( tcp_cam_pub_client ${OpenCV_LIBS} )
target_link_libraries( tcp_cam_sub_server ${OpenCV_LIBS} )
# target_link_libraries( send_img_tcp ${OpenCV_LIBS} )
# target_link_libraries( recv_img_tcp ${OpenCV_LIBS} )

# target_link_libraries( tcp_cam_client ${OpenCV_LIBS} )
# target_link_libraries( tcp_cam_server ${OpenCV_LIBS} )
# target_link_libraries( udp_client ${OpenCV_LIBS} )
# target_link_libraries( udp_server ${OpenCV_LIBS} )
