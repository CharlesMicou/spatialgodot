// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#ifndef WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DEFAULTS_H
#define WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DEFAULTS_H
#include <cstddef>
#include <cstdint>
#include <string>

namespace worker {
namespace defaults {
namespace detail {
// Helper to avoid having a separate std::string object per translation unit.
inline const std::string& LogPrefix() {
  static const std::string kLogPrefix = "protocol-log-";
  return kLogPrefix;
}
}  // ::detail
// General asynchronous IO.
constexpr std::uint32_t kSendQueueCapacity = 4096;
constexpr std::uint32_t kReceiveQueueCapacity = 4096;
constexpr std::uint32_t kLogMessageQueueCapacity = 256;
constexpr std::uint32_t kBuiltInMetricsReportPeriodMillis = 5000;
// General networking.
constexpr bool kUseExternalIp = false;
constexpr std::uint32_t kConnectionTimeoutMillis = 60000;
// TCP.
constexpr std::uint8_t kTcpMultiplexLevel = 32;
constexpr std::uint32_t kTcpSendBufferSize = 65536;
constexpr std::uint32_t kTcpReceiveBufferSize = 65536;
constexpr bool kTcpNoDelay = false;
// RakNet.
constexpr std::uint32_t kRakNetHeartbeatTimeoutMillis = 60000;
// Protocol logging.
static const auto& kLogPrefix = detail::LogPrefix();
constexpr std::uint32_t kMaxLogFiles = 10;
constexpr std::uint32_t kMaxLogFileSizeBytes = 1024 * 1024;
}  // ::defaults
}  // ::worker

#endif  // WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DEFAULTS_H
