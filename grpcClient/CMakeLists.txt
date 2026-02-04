# -------------- 1. 基础配置 --------------
cmake_minimum_required(VERSION 3.16)

# 项目名称建议和目标一致，避免混淆
project(GrpcClientProject CXX)

# -------------- 2. C++ 标准 --------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------- 3. 依赖查找配置 (关键优化) --------------
# 技巧：不要在 CMake 文件里写死 "/home/ttw/..." 绝对路径。
# 而是告诉 CMake："如果在系统默认路径找不到，就去这个路径看看"
# 这样你的代码发给别人（或者换台电脑）也能跑。
list(APPEND CMAKE_PREFIX_PATH "/home/fd/GRPC_Source/grpc/cmake/build/install")

# 查找 Abseil (现代 gRPC 强依赖 absl，建议加上以防链接错误)
find_package(absl CONFIG REQUIRED)

# 查找 Protobuf
find_package(protobuf CONFIG REQUIRED)

# 查找 gRPC
find_package(gRPC CONFIG REQUIRED)

# -------------- 4. 源码文件管理 --------------
# 定义变量管理源文件，更加清晰
set(PROTO_SRC
    stream_proto/proto/stream.pb.cc
    stream_proto/proto/stream.grpc.pb.cc
)

set(APP_SRC
    mainClient.cpp
)

# -------------- 5. 创建可执行目标 --------------
add_executable(grpc_client ${APP_SRC} ${PROTO_SRC})

# -------------- 6. 头文件包含路径 --------------
target_include_directories(grpc_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    # 如果你的代码里写的是 #include "stream.pb.h" 而不是 #include "stream_proto/proto/stream.pb.h"
    # 你可能需要把这个子目录也加进来：
    ${CMAKE_CURRENT_SOURCE_DIR}/stream_proto/proto
)

# -------------- 7. 链接库 --------------
target_link_libraries(grpc_client PRIVATE
    gRPC::grpc++
    gRPC::grpc
    protobuf::libprotobuf # 注意：新版 Protobuf 推荐用这个 target
    absl::base            # 基础 Abseil 支持 (防止某些版本报错)
    absl::strings         # 常用字符串支持
)

# 也就是不需要显式链接 pthread，CMake 会自动处理 gRPC 的线程依赖

# -------------- 8. 调试信息 --------------
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Protobuf Version: ${protobuf_VERSION}")
message(STATUS "gRPC Version: ${gRPC_VERSION}")