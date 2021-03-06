/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <sys/time.h>

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientWriter;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

  std::string SayHelloStream(const std::string& user){
  	ClientContext context;
	HelloRequest request;
	request.set_name(user);
	HelloReply reply;
	std::unique_ptr<ClientWriter<HelloRequest> > writer(stub_->SayHelloStream(&context, &reply));
	for (int i=0;i<100;i++){
		if(!writer->Write(request)){
			break;
		}
	}
	writer->WritesDone();
	Status status = writer->Finish();
	if (status.ok()) {
		return reply.message();
	} else {
	        std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
		return "RPC failed";
	}
  }
 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
  struct timeval start, end;
  long secs, usecs;
  float mtime;
  GreeterClient greeter(grpc::CreateChannel(
      "129.97.167.80:8080", grpc::InsecureChannelCredentials()));
  //For Q4
  for (int i=0; i<100; i++){
	gettimeofday(&start, NULL);
	std::string user("hello");
	std::string reply = greeter.SayHello(user);
	gettimeofday(&end, NULL);
	secs = end.tv_sec - start.tv_sec;
	usecs = end.tv_usec - start.tv_usec;
	mtime = ((secs) * 1000 + usecs/1000.0);
	std::cout << mtime <<std::endl;
	//std::cout << "Greeter received: " << reply << std::endl;
  }
/*
  //For Q6
  for (int i=0; i<100; i++){
	gettimeofday(&start, NULL);
	//std::string user(4000000,'0');
	//std::string reply = greeter.SayHello(user);
	//streaming
	std::string user(40000, '0');
	std::string reply = greeter.SayHelloStream(user);
	gettimeofday(&end, NULL);
	secs = end.tv_sec - start.tv_sec;
	usecs = end.tv_usec - start.tv_usec;
	mtime = ((secs) * 1000 + usecs/1000.0);
	std::cout << mtime <<std::endl;
  }
 */
  return 0;
}
