#ifndef _THRIFTEX_BASE_LOGGING_H_
#define _THRIFTEX_BASE_LOGGING_H_

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <glog/raw_logging.h>

#include <string>

namespace thriftex {
namespace base {

/* Logging with inner mutex  */
// Debug mode, not present in release build
#define EX_DEBUG_INFO DLOG(INFO) << "[DEBUG]  "
// Debug / Release mode
#define EX_INFO  LOG(INFO)
#define EX_WARN  LOG(WARNING)
#define EX_ERROR LOG(ERROR)
#define EX_FATAL LOG(FATAL)            // Will core-dump when use this

/* Logging raw type, lock-free */
#define EX_RAW_INFO(...)  RAW_LOG_INFO(__VA_ARGS__)
#define EX_RAW_WARN(...)  RAW_LOG_WARNING(__VA_ARGS__)
#define EX_RAW_ERROR(...) RAW_LOG_ERROR(__VA_ARGS__)
#define EX_RAW_FATAL(...) RAW_LOG_FATAL(__VA_ARGS__) // Will core-dump

static void set_ex_logging_file(const char* log_name) {
    google::InitGoogleLogging(log_name);
}

}}

#endif /* LOGGING_H_ */
