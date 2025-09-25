# -------------- 基础要求 --------------
# 最低 CMake 版本；3.16 以上对 gRPC 支持更好
cmake_minimum_required(VERSION 3.16)
project(GrpcMainServer CXX)

# -------------- C++ 标准 --------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------- 寻找依赖 --------------


# 1. 告诉 CMake 去哪儿找 <Package>Config.cmake
# set(protobuf_DIR "/home/ttw/.local/lib/cmake/protobuf" CACHE PATH "")
# set(gRPC_DIR     "/home/ttw/.local/lib/cmake/grpc"     CACHE PATH "")

# -------------- 寻找依赖 --------------
# 强制使用 26.1 自带的配置文件，禁用系统旧模块
find_package(protobuf CONFIG REQUIRED
             PATHS /home/ttw/.local/lib/cmake/protobuf
             NO_DEFAULT_PATH)
find_package(gRPC     CONFIG REQUIRED
             PATHS /home/ttw/.local/lib/cmake/grpc
             NO_DEFAULT_PATH)
# 2. 强制使用 CONFIG 模式，禁用旧 Find 模块
# find_package(protobuf CONFIG REQUIRED)   # 全小写
# find_package(gRPC     CONFIG REQUIRED)   # 正常大小写

# -------------- 整理源码 --------------
# 把要编译的源文件列出来；保持和你手动 g++ 时一致
set(SOURCES
    mainClient.cpp          # 你自己的服务端主程序
    stream_proto/proto/stream.pb.cc # protoc  生成的消息类
    stream_proto/proto/stream.grpc.pb.cc # grpc_cpp_plugin 生成的服务类、
)

# -------------- 创建可执行目标 --------------

add_executable(mainClient ${SOURCES})

# -------------- 头文件搜索路径 --------------
# 让编译器能找到 .proto 生成的头文件（当前目录）
target_include_directories(mainClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# -------------- 链接库 --------------
# 需要链接的库：grpc++、grpc、protobuf、线程
target_link_libraries(mainClient
    gRPC::grpc++
    gRPC::grpc
    protobuf::libprotobuf   # 26.1 的真实目标，没有 ::protobuf 别名
    pthread
)

# -------------- 可选：打印调试信息 --------------
message(STATUS "Protobuf version : ${Protobuf_VERSION}")
message(STATUS "gRPC version       : ${gRPC_VERSION}")