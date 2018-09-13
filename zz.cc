#include <memory>
#include <string>

#include <grpc/grpc.h>

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/client_context.h>

#include <grpcpp/grpcpp.h>

#include "rpc.grpc.pb.h" 
#include "rpc.pb.h" 

using namespace etcdserverpb;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;

int main()
{
     grpc_init();
     auto channel = grpc::CreateChannel("127.0.0.1:2379", grpc::InsecureChannelCredentials());

     std::cout<<channel.use_count();

     std::unique_ptr<KV::Stub> stub(KV::NewStub(channel));

     ClientContext context;
     PutRequest request;
     PutResponse response;

     request.set_key("/liuwei");
     request.set_value("value");

     // Set
     auto ret = stub->Put(&context, request, &response);

     if ( ret.ok() ) {
         std::cout<<"ok\n";
         std::cout<<"header"<<response.has_header()<<std::endl;
         std::cout<<"prev_kv"<<response.has_header()<<std::endl;
         std::cout<<"prev_k"<<response.prev_kv().key()<<std::endl;
         std::cout<<"prev_v"<<response.prev_kv().value()<<std::endl;
     } else {
         std::cout<<"fail\n";
         std::cout<<ret.error_code()<<std::endl;
         std::cout<<ret.error_message()<<std::endl;
         std::cout<<ret.error_details()<<std::endl;
     }
 
     // Get
     RangeRequest request1;
     RangeResponse response1;
     ClientContext context1;

     request1.set_key("/liuwei");
     ret = stub->Range(&context1, request1, &response1);

     if ( ret.ok() ) {
         std::cout<<"ok\n";
         std::cout<<response1.count()<<std::endl;
         std::cout<<response1.kvs()[0].key()<<std::endl;
         std::cout<<response1.kvs()[0].value()<<std::endl;
     } else {
         std::cout<<"fail\n";
         std::cout<<ret.error_code()<<std::endl;
         std::cout<<ret.error_message()<<std::endl;
         std::cout<<ret.error_details()<<std::endl;
     }    

     // Watch
     std::unique_ptr<Watch::Stub> watch(Watch::NewStub(channel));

     if ( watch ) {
         std::cout<<"watch ok"<<std::endl;
     } else {
         std::cout<<"watch fail"<<std::endl;
     }

     ClientContext context2;
     WatchRequest req;
     WatchResponse res;

     std::shared_ptr<ClientReaderWriter<WatchRequest, WatchResponse>> stream(watch->Watch(&context2));

     stream->Write(req);
     stream->WritesDone();
     stream->Finish();

     std::cout<<"watch finish"<<std::endl;

     stream->Read(&res);
}
