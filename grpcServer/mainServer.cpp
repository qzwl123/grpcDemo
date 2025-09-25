/*************************************************************************
	> File Name: mainServer.cpp
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


class MygrpcServer final : public RouteGuide::Service {

    // 一元RPC
    virtual grpc::Status sayHello(grpc::ServerContext* context, const routeguide::Request* request, routeguide::Response* response) {
        std::cout << CLR_GREEN << " [ sayHello ] : " << CLR_NONE << " id :" << request->id() << " name :" << request->name() << std::endl;
        std::ostringstream message;
        message << " sayHello Name : " << request->name();
        response->set_message(message.str());

        return grpc::Status::OK;
    }


    // 服务器流RPC
    virtual grpc::Status ListFeatures(grpc::ServerContext* context, const routeguide::Request* request, grpc::ServerWriter< routeguide::Response >* writer) {
        std::cout << CLR_GREEN << " [ 服务器流RPC_ListFeatures ] : " << CLR_NONE << " id :" << request->id() << " name :" << request->name() << std::endl;

        // id 号累加 10次
        get_m_Meassage_list_(*request);

        for(const Response& r : m_Meassage_list_) {
            writer->Write(r);
        }

        return grpc::Status::OK;
    }

    // 客户端流PRC
    virtual grpc::Status RecordRoute(grpc::ServerContext* context, grpc::ServerReader< routeguide::Request>* reader, ::routeguide::Response* response) {
        // std::cout << CLR_GREEN << " [ 客户端流PRC_RecordRoute ] : " << CLR_NONE << " id :" << request->id() << " name :" << request->name() << std::endl;

        return grpc::Status::OK;
    }

    // 双向流RPC
    virtual grpc::Status RouteChat(grpc::ServerContext* context, grpc::ServerReaderWriter< routeguide::Response, ::routeguide::Request>* stream) {
        // std::cout << CLR_GREEN << " [ 双向流RPC_RouteChat ] : " << CLR_NONE << " id :" << request->id() << " name :" << request->name() << std::endl;

        return grpc::Status::OK;
    }

 private:
  std::vector<Response> m_Meassage_list_;

 private:
    void get_m_Meassage_list_(const Request &rest) {
        m_Meassage_list_ = std::vector<Response>();
        for(int i = 0; i < 10; i += 1) {
            Response t;
            t.set_message(std::to_string(rest.id() + 1) + "_" + rest.name());            
            m_Meassage_list_.push_back(t);

        }
        return ;
    }

};


void runServer(const char* ipPort = "0.0.0.0:5200") {
    std::cout << CLR_CYAN << " [ runServer ] : "  << CLR_NONE << ipPort << std::endl;

    // 1.设置服务程序参数
    grpc::ServerBuilder buider;
    // 第二个参数表示不使用安全连接
    buider.AddListeningPort(ipPort, grpc::InsecureServerCredentials());

    // 2.注册服务函数
    MygrpcServer service;
    buider.RegisterService(&service);

    // 3.启动服务器
    std::unique_ptr<grpc::Server> server = buider.BuildAndStart();
    server->Wait();
    return ;
}

int main() {
    std::cout << CLR_CYAN << " [ main runServer Ordinary ] " << CLR_NONE << std::endl;
    runServer();
    return 0;
}

