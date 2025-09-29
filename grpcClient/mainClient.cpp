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
#include <condition_variable>

#include <grpcpp/grpcpp.h>
#include "color.h"

#include "stream_proto/proto/stream.grpc.pb.h"

/********************* 公共头文件 End *********************/

/********************* using Proto Beggin *********************/

using routeguide::Response;
using routeguide::Request;

using routeguide::RouteGuide;

/********************* using Proto End *********************/

BEGINS(synchronize_GRPC)

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

    std::cout << " ********* 同步 RPC Begin ********* " << std::endl;
    std::cout << " 1 . 一元 RPC " << std::endl;
    std::cout << " 2 . 服务器流RPC " << std::endl;
    std::cout << " 3 . 客户端流RPC " << std::endl;
    std::cout << " ********* 同步 RPC Endl ********* " << std::endl;

    int nu = 0;
    std::cin >> nu;
    switch (nu)
    {
    case 1:
        guide.GetFeature();
        break;
    case 2:
        guide.ListFeatures();
        break;
    case 3:
        guide.RecordRoute();
        break;
    default:
        break;
    }
    return 0;
}
ENDS(synchronize_GRPC)








BEGINS(callback_GRPC)

class RouteGuideClient {
public:
    RouteGuideClient(std::shared_ptr<grpc::Channel> channel) : 
                    m_stub_(RouteGuide::NewStub(channel)) {

    }                    

    
#if true

    class HelloReactor : public grpc::ClientUnaryReactor {        
        public:

            HelloReactor(const std::shared_ptr<RouteGuide::Stub>& stub) :
                        stub_(stub), 
                        ctx_(new grpc::ClientContext),
                        request_(new routeguide::Request),
                        response_(new routeguide::Response) {          

                // 请求
                request_->set_id(1);
                request_->set_data("Syahellow");


                // 把 this 注册为 reactor，框架会在完成时回调 OnDone
                stub_->async()->sayHello(ctx_.get(), request_.get(), response_.get(), this);
                // 2. 关键：把请求发出去！
                StartCall();   // ← 少了这一步，框架永远不会回调
            }

            /* ---------- 框架回调 ---------- */
            void OnDone(const grpc::Status& status) override {
                
                if (status.ok()) {
                    std::cout << CLR_GREEN << " [ sayHello -> HelloReactor ] :" << CLR_NONE << std::hex << response_->message() << std::endl;
                } else {
                    std::cout << CLR_RED <<  " [ sayHello -> HelloReactor ] :"  << status.error_message() << std::endl;
                }
                delete this;   // 自杀，生命周期结束
            }

        private:
            std::shared_ptr<RouteGuide::Stub> stub_;  // 如果 stub 全局可省
            std::unique_ptr<grpc::ClientContext>          ctx_;
            std::unique_ptr<routeguide::Request>          request_;
            std::unique_ptr<routeguide::Response>         response_;
    };

    /// @brief 一元RPC 对象回调
    void *sayHello() {
        printFuncName("sayHello");
        return new HelloReactor(m_stub_);   // 立即返回，0 阻塞        
    }   

#else
    /// @brief 一元RPC 匿名函数（闭包）
    void sayHello() {
        Request request;
        Response response;
        grpc::ClientContext context;

        request.set_id(1);
        request.set_data("sayHello_匿名函数");
        
        bool result;
        std::mutex mu;
        std::condition_variable cv;
        bool done = false;
        m_stub_->async()->sayHello(
        &context, &request, &response,
        [&result, &mu, &cv, &done, response, this](grpc::Status status) {
          bool ret;
          if (!status.ok()) {
            std::cout << "GetFeature rpc failed." << std::endl;
            ret = false;
          }  else {
            std::cout << "Found feature called " 
                      << response.message()
                      << std::endl;
            ret = true;
          }
          std::lock_guard<std::mutex> lock(mu);
          result = ret;
          done = true;
          cv.notify_one();
        });
        std::unique_lock<std::mutex> lock(mu);
        cv.wait(lock, [&done] { return done; });
        return ;
    }
#endif    


    /// @brief 服务器流RPC
    void ListFeatures() {}
    /// @brief 客户端流RPC  
    void RecordRoute() {}    

private:
    std::shared_ptr< RouteGuide::Stub> m_stub_;
    
    void printFuncName(std::string s) {
        std::cout << CLR_CYAN << " [ " << s << " ] " << CLR_NONE << std::endl;
    }
};

int main() {
    RouteGuideClient guide(
        grpc::CreateChannel("127.0.0.0:5200", grpc::InsecureChannelCredentials())
    );

    std::cout << " ********* 回调 RPC Begin ********* " << std::endl;
    std::cout << " 1 . 一元 RPC " << std::endl;
    std::cout << " 2 . 服务器流RPC " << std::endl;
    std::cout << " 3 . 客户端流RPC " << std::endl;
    std::cout << " ********* 回调 RPC Endl ********* " << std::endl;

    int nu = 0;
    while (std::cin >> nu) {        
        switch (nu)
        {
        case 1:
            guide.sayHello();
            break;
        case 2:
            guide.ListFeatures();
            break;
        case 3:
            guide.RecordRoute();
            break;
        default:
            break;
        }
    }
    

    return 0;
}
ENDS(callback_GRPC)

int main() {

    // synchronize_GRPC::main();

    callback_GRPC::main();

    return 0;
}