cmake_minimum_required(VERSION 3.16)
project(test)

list(APPEND CMAKE_PREFIX_PATH "/home/ttw/.local")
find_package(Protobuf REQUIRED CONFIG) # 26.1
find_package(absl   REQUIRED)          # 20240116

add_executable(test Mytesst.cpp Person.pb.cc)
target_link_libraries(test PRIVATE
        protobuf::libprotobuf
        absl::strings absl::time absl::status
        absl::synchronization absl::cord absl::log absl::hash
        absl::base absl::debugging_internal)