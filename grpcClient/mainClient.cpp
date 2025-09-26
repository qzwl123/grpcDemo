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
#include <thread>

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

    /// @brief 一元RPC
    void GetFeature() {        
        printFuncName("GetFeature");

        Request requt; 
        Response resp;
        
        requt.set_id(1);
        requt.set_data("ttw");
        
        grpc::ClientContext context;
        grpc::Status status = m_stub_->sayHello(&context, requt, &resp);

        if(!status.ok()) {
            std::cout << CLR_RED << " [ GetFeature ] " << CLR_NONE << "Server returns incomplete feature." << std::endl;
        }

        return ;
    }
    
    /// @brief 服务器流RPC
    void ListFeatures() {
        printFuncName("ListFeatures");

        Request requt; 
        Response resp;
        grpc::ClientContext context;

        requt.set_id(1);
        requt.set_data("ttw");

        std::unique_ptr<grpc::ClientReader<Response> > reader(
        m_stub_->ListFeatures(&context, requt));

        while (reader->Read(&resp)) // 阻塞等待，来一条处理一条
        {
            std::cout << CLR_GREEN << "[ ListFeatures ]" << CLR_NONE << " : " << resp.message() << std::endl;
        }
        grpc::Status status = reader->Finish();

        if (status.ok()) {
            std::cout << CLR_CYAN << "[ ListFeatures ]" << CLR_NONE << "ListFeatures rpc succeeded." << std::endl;
        } else {
            std::cout << CLR_RED << "[ ListFeatures ]" << CLR_NONE << "ListFeatures rpc failed." << std::endl;
        }
        return ;
    }
    
    /// @brief 客户端流RPC  
    void RecordRoute() {
        printFuncName("RecordRoute");

        Request requt; 
        Response resp;
        grpc::ClientContext context;

        requt.set_id(1);
        requt.set_data("RecordRoute");

        std::unique_ptr<grpc::ClientWriter<Request> > writer(
            m_stub_->RecordRoute(&context, &resp));
        int i = 0;
        for(i = 0; i < 10; i += 1) {
        
            requt.set_id(i);
            if(!writer->Write(requt)) {
                // false 表示流已关闭。
                break; 
            }
            
            /* 
                为什么在同步模式上加上 sleep 后一条都发不出去

                在 同步客户端流 模式下，Write() 只是把对象序列化后塞进 gRPC 的用户态缓冲区，
                真正的 TCP send() 由 gRPC 的内部 “发送协程” 负责；而这个协程和调用 Write() 的线程是同一个（同步完成队列没有额外后台线程）。
                std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // 毫秒  只挂当前线程，单位自带类型安全
            */

        }
        std::cout << CLR_CYAN << " [ RecordRoute ] Send count : " << CLR_NONE  << i << std::endl;
        writer->WritesDone();

    }

 private :
    void printFuncName(std::string s) {
        std::cout << CLR_CYAN << " [ " << s << " ] " << CLR_NONE << std::endl;
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

    guide.ListFeatures();

    guide.RecordRoute();
    return 0;
}