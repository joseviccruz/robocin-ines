//
// Created by jose-cruz on 10/03/23.
//

#include "ines/middleware/middleware.h"

namespace ines::middleware {
namespace {

using ::grpc::ServerContext;
using ::grpc::Status;

} // namespace

Status MiddlewareServiceImpl::SetVision([[maybe_unused]] ServerContext* context,
                                        const SetVisionRequest* request,
                                        [[maybe_unused]] SetVisionResponse* response) {
  const absl::WriterMutexLock kLock(&vision_mutex_);
  vision_data_ = request->vision();
  return Status::OK;
}

Status MiddlewareServiceImpl::GetVision([[maybe_unused]] ServerContext* context,
                                        [[maybe_unused]] const GetVisionRequest* request,
                                        GetVisionResponse* response) {
  const absl::ReaderMutexLock kLock(&vision_mutex_);
  response->mutable_vision()->CopyFrom(vision_data_);
  return Status::OK;
}

Status MiddlewareServiceImpl::SetReferee([[maybe_unused]] ServerContext* context,
                                         const SetRefereeRequest* request,
                                         [[maybe_unused]] SetRefereeResponse* response) {
  const absl::WriterMutexLock kLock(&referee_mutex_);
  referee_data_ = request->referee();
  return Status::OK;
}

Status MiddlewareServiceImpl::GetReferee([[maybe_unused]] ServerContext* context,
                                         [[maybe_unused]] const GetRefereeRequest* request,
                                         GetRefereeResponse* response) {
  const absl::ReaderMutexLock kLock(&referee_mutex_);
  response->mutable_referee()->CopyFrom(referee_data_);
  return Status::OK;
}

} // namespace ines::middleware