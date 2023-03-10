//
// Created by jose-cruz on 10/03/23.
//

#include <string>

#include <grpcpp/grpcpp.h>

#include "ines/middleware/middleware.h"

using grpc::InsecureServerCredentials;
using grpc::Server;
using grpc::ServerBuilder;
using ines::MiddlewareServiceImpl;

void runServer() {
  const std::string kServerAddress("0.0.0.0:50051");
  MiddlewareServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(kServerAddress, InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << kServerAddress << std::endl;
  server->Wait();
}

int main() {
  runServer();
  return 0;
}