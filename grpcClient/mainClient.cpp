/*************************************************************************
	> File Name: mainClient.cpp
	> Author: ttw
	> Mail: 1658339000@qq.com
	> Created Time: 2025年09月25日 星期四 10时27分51秒
 ************************************************************************/

#define BEGINS(x) namespace x{
#define ENDS(x) } // end of namespace x 

/********************* 公共头文件 Beggin *********************/

#include <iostream>
#include <string>
#include <vector>
#include <memory> 

#include <grpcpp/grpcpp.h>
#include "color.h"

#include "stream_proto/proto/stream.grpc.pb.h"

/********************* 公共头文件 End *********************/

/********************* using Proto Beggin *********************/

using routeguide::Response;
using routeguide::Request;

using routeguide::RouteGuide;

/********************* using Proto End *********************/

class RouteGuideClient{
 public:
    RouteGuideClient(std::shared_ptr<grpc::Channel> channel) : m_stub_(RouteGuide::NewStub(channel)) {

    }

    void GetFeature() {
        std::cout << CLR_CYAN << " [ GetFeature ] " << CLR_NONE << std::endl;

        Request requt; 
        Response resp;
        
        requt.set_id(1);
        requt.set_name("ttw");
        
        grpc::ClientContext context;
        grpc::Status status = m_stub_->sayHello(&context, requt, &resp);

        if(!status.ok()) {
            std::cout << CLR_RED << " [ GetFeature ] " << CLR_NONE << "Server returns incomplete feature." << std::endl;
        }

        return ;
    }

 private:
    // 创建远程调用代理
    std::unique_ptr< RouteGuide::Stub> m_stub_;

};

#if false
void runClient() {

    // 构建通道
    std::shared_ptr<grpc::Channel> channel =  grpc::CreateChannel("127.0.0.1:5200", grpc::InsecureChannelCredentials());
    
    // 创建远程调用代理
    std::unique_ptr< grpcS::Stub> stub= grpcS::NewStub(channel);
    
    // 通过代理元从调用服务端函数
    grpc::ClientContext context;

    Request request;
    request.set_id(1);
    request.set_age(18);
    request.set_name("ttw");

    Response response;
    grpc::Status status = stub->sayHello(&context, request, &response);

    if(status.ok() ) {
        std::cout << response.message() << std::endl;
    } else {
        std::cerr << status.error_code() << " " << status.error_message() << std::endl;
    }

    return ;
}
#endif

int main() {
    RouteGuideClient guide(
        grpc::CreateChannel("127.0.0.0:5200", grpc::InsecureChannelCredentials())
    );

    guide.GetFeature();

    return 0;
}