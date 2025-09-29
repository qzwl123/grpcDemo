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

// 同步RPC
BEGINS(synchronize_GRPC)

class MygrpcServer final : public RouteGuide::Service {

    
    // 一元RPC
    virtual grpc::Status sayHello(grpc::ServerContext* context, const routeguide::Request* request, routeguide::Response* response) {
        std::cout << CLR_GREEN << " [ sayHello ] : " << CLR_NONE << " id :" << request->id() << " name :" << request->data() << std::endl;
        std::ostringstream message;
        message << " sayHello Name : " << request->data();
        response->set_message(message.str());

        return grpc::Status::OK;
    }

    // 服务器流RPC
    virtual grpc::Status ListFeatures(grpc::ServerContext* context, const routeguide::Request* request, grpc::ServerWriter< routeguide::Response >* writer) {
        std::cout << CLR_GREEN << " [ 服务器流RPC_ListFeatures ] : " << CLR_NONE << " id :" << request->id() << " name :" << request->data() << std::endl;

        // id 号累加 10次
        get_m_Meassage_list_(*request);

        for(const Response& r : m_Meassage_list_) {
            writer->Write(r); // 随时发，HTTP/2 DATA 帧
        }

        // writer->WritesDone(); 同步 ServerWriter 接口在函数返回时，gRPC 会自动补调 WritesDone()
        return grpc::Status::OK;
    }

    // 客户端流PRC
    virtual grpc::Status RecordRoute(grpc::ServerContext* context, grpc::ServerReader< routeguide::Request>* reader, ::routeguide::Response* response) {
        std::cout << CLR_CYAN << " [ 客户端流PRC_RecordRoute ]  " << std::endl;

        Request requt; 
        Response resp;
        int Request_count = 0;

        while (reader->Read(&requt)) {
            Request_count++;
            std::cout << CLR_GREEN << " [ RecordRoute Recv ] " << CLR_NONE << " : " << requt.id() << " " << requt.data() << std::endl;
        
        }
        
        std::cout << CLR_CYAN << " [ 客户端流PRC_END messages count ] : " << CLR_NONE <<  Request_count << std::endl;
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
            std::string s = std::to_string(rest.id() + i) + "_" + rest.data();
            t.set_message(s);            
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

ENDS(synchronize_GRPC)

/***************************************************************************************************************************************************/

BEGINS(callback_GRPC)


/*

    返回 ServerUnaryReactor* 就是向框架声明：“我玩的是异步回调，生命周期我自己管，等我 Finish() 你再发响应。”

*/

class MygrpcServer final : public RouteGuide::CallbackService{

    /// @brief 一元RPC
    /// @param context 
    /// @param request 
    /// @param response 
    /// @return 
    grpc::ServerUnaryReactor* sayHello(grpc::CallbackServerContext* context, 
                                       const ::routeguide::Request* request, 
                                       routeguide::Response* response)  { 

        class Reactor : public  grpc::ServerUnaryReactor {

        public:
            Reactor(const Request &request, Response *response) // 初始化列表，可以传进来 业务所需要 变量
            {
                std::cout << CLR_GREEN << " [ sayHello ] : " << CLR_NONE << request.id() << " " << request.data() << std::endl;

                /*********************** 业务处理逻辑 Begin ***********************/

                response->set_message("sayHello");
                
                Finish(grpc::Status::OK); // 会触发 OnDone() 一次且仅一次

                /*********************** 业务处理逻辑 End ***********************/
            }

        private:
            // 当 最后一次写（Finish）也完成 且 连接层彻底清理 后，框架回调 OnDone()
            void OnDone() override {
                std::cout << CLR_CYAN << "[ sayHello  OnDone ] INFO : " << CLR_NONE << "RPC Completed" << std::endl;                
                delete this;
            }

            // 当 OnWriteDone ok=false / 客户端按 Ctrl-C 中途杀流 ， 当前函数执行完，最后也会调用  OnDone()
            void OnCancel() override { 
                std::cout << CLR_RED << "[ sayHello OnCancel] ERROR : " << CLR_NONE << "RPC Cancelled" << std::endl;                
            }

        };


        return new Reactor(*request, response); 
    }

    /// @brief 服务器流RPC
    /// @param context 
    /// @param request 
    /// @return 
    grpc::ServerWriteReactor<routeguide::Response>* ListFeatures(grpc::CallbackServerContext* context, const ::routeguide::Request* request)  { 
        return nullptr; 
    }

    /// @brief 客户端流PRC
    /// @param context 
    /// @param response 
    /// @return 
    grpc::ServerReadReactor<routeguide::Request>* RecordRoute(grpc::CallbackServerContext* context, ::routeguide::Response* response)  { 
        return nullptr; 
    }

    /// @brief 双向流RPC
    /// @param context 
    /// @return 
    grpc::ServerBidiReactor<routeguide::Request, routeguide::Response>* RouteChat(grpc::CallbackServerContext* context) {
        return nullptr; 

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
    std::cout << CLR_CYAN << " [ 回调 main runServer Ordinary ] " << CLR_NONE << std::endl;
    runServer();

    return 0;
}

ENDS(callback_GRPC)

int main() {
    // 同步测试 : RPC server
    // synchronize_GRPC::main();

    // 回调测试 ： RPC server
    callback_GRPC::main();

    return 0;
}

