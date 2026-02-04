protoc --cpp_out=./stream_proto/proto \
       --grpc_out=./stream_proto/proto \
       --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
       -I./stream_proto \
       ./stream_proto/stream.proto 

#cmake -S . -B build      # 配置
#cmake --build build      # 编译

