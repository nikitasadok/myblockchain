cmake_minimum_required(VERSION 3.19)
project(blockchain)

set(CMAKE_CXX_STANDARD 14)

add_executable(blockchain main.cpp src/Transaction.cpp src/Transaction.h src/Block.cpp src/Block.h src/Blockchain.cpp src/Blockchain.h src/Client.cpp src/Client.h)
target_link_libraries(blockchain /usr/lib/x86_64-linux-gnu/libcrypto++.a)