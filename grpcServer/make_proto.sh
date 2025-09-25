protoc --cpp_out=./stream_proto/proto \
       --grpc_out=./stream_proto/proto \
       --plugin=protoc-gen-grpc="/home/ttw/.local/bin/grpc_cpp_plugin" \
       -I./stream_proto \
       ./stream_proto/stream.proto
