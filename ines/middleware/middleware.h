//
// Created by jose-cruz on 10/03/23.
//

#ifndef INES_MIDDLEWARE_H
#define INES_MIDDLEWARE_H

#include <absl/synchronization/mutex.h>

#include "ines/middleware/middleware.grpc.pb.h"

namespace ines::middleware {

class MiddlewareServiceImpl final : public Middleware::Service {
 public:
  ::grpc::Status SetVision(::grpc::ServerContext* context,
                           const SetVisionRequest* request,
                           SetVisionResponse* response) override;
  ::grpc::Status GetVision(::grpc::ServerContext* context,
                           const GetVisionRequest* request,
                           GetVisionResponse* response) override;

  ::grpc::Status SetReferee(::grpc::ServerContext* context,
                            const SetRefereeRequest* request,
                            SetRefereeResponse* response) override;
  ::grpc::Status GetReferee(::grpc::ServerContext* context,
                            const GetRefereeRequest* request,
                            GetRefereeResponse* response) override;

 private:
  absl::Mutex vision_mutex_;
  common::messages::Vision vision_data_;

  absl::Mutex referee_mutex_;
  common::messages::Referee referee_data_;
};

} // namespace ines::middleware

#endif // INES_MIDDLEWARE_H
