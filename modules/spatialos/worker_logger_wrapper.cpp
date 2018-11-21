#include "worker_logger_wrapper.h"
#include "spatial_util.h"

WorkerLogger WorkerLoggerWrapper::logger = WorkerLogger("gdscript");

void WorkerLoggerWrapper::_bind_methods() {
    ClassDB::bind_method(D_METHOD("info"), &WorkerLoggerWrapper::info);
    ClassDB::bind_method(D_METHOD("warn"), &WorkerLoggerWrapper::warn);
    ClassDB::bind_method(D_METHOD("error"), &WorkerLoggerWrapper::error);
}

void WorkerLoggerWrapper::info(const String& msg) {
    WorkerLoggerWrapper::logger.info(fromGodotString(msg));
}

void WorkerLoggerWrapper::warn(const String& msg) {
    WorkerLoggerWrapper::logger.warn(fromGodotString(msg));
}

void WorkerLoggerWrapper::error(const String& msg) {
    WorkerLoggerWrapper::logger.error(fromGodotString(msg));
}

WorkerLoggerWrapper::WorkerLoggerWrapper() {
}
