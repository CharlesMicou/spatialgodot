#ifndef WORKER_SDK_CPP_INCLUDE_PRIVATE_MAVERICKS_H
#define WORKER_SDK_CPP_INCLUDE_PRIVATE_MAVERICKS_H
#include <improbable/collections.h>
#include <improbable/detail/connection.i.h>
#include <improbable/worker.h>

namespace worker {
namespace alpha {

inline void RecordBinaryLogEvent(const Connection& connection) {
  detail::internal::WorkerProtocol_Connection_RecordBinaryLogEvent(connection.connection.get());
}

inline void RecordBinaryLogEventMessage(const Connection& connection, const std::string& message) {
  detail::internal::WorkerProtocol_Connection_RecordBinaryLogEventMessage(
      connection.connection.get(), message.c_str());
}

}  //  ::alpha
}  //  ::worker

#endif  // WORKER_SDK_CPP_INCLUDE_PRIVATE_MAVERICKS_H
