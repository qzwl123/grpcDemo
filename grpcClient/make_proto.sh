protoc --cpp_out=./stream_proto/proto \
       --grpc_out=./stream_proto/proto \
       --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
       -I./stream_proto \
       ./stream_proto/stream.proto

#  cmake -S . -B build/
#  cmake --build build/

#protoc --cpp_out=./stream_proto/proto \
#       --grpc_out=./stream_proto/proto \
#       --plugin=protoc-gen-grpc="/home/fd/GRPC_Source/grpc/cmake/build/install/bin/grpc_cpp_plugin" \
#       -I./stream_proto \
#       ./stream_proto/stream.proto
