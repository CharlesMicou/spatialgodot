#include "worker_logger.h"
#include <iostream>
#include <fstream>
#include <improbable/worker.h>
#include "core/os/memory.h"
#include <time.h>

#ifndef LOG_COLORS
#define LOG_COLORS

#define ERROR_STR "error"
#define WARN_STR "warn"
#define INFO_STR "info"

#define RED_MOD "\033[31m"
#define YELLOW_MOD "\033[33m"
#define WHITE_MOD "\033[37m"
#define RESET_MOD "\033[0m"

#endif

// C++ is still a total mystery to me.
std::ofstream* WorkerLogger::log_file = NULL;
std::function<void(const worker::LogLevel, const std::string&, const std::string&)> WorkerLogger::logback = NULL;
int WorkerLogger::log_to_connection_severity;
int WorkerLogger::log_to_console_severity;

inline std::string makeTimestamp() {
    // todo: use chrono to use ms
    time_t now = time(NULL);
    struct tm tstruct;
    char buf[40];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%X", &tstruct);
    return buf;
}

void WorkerLogger::info(const std::string& msg) {
    write(log_severity::INFO, logger_name, msg);
}

void WorkerLogger::warn(const std::string& msg) {
    write(log_severity::WARN, logger_name, msg);
}

void WorkerLogger::error(const std::string& msg) {
    write(log_severity::ERROR, logger_name, msg);
}

void WorkerLogger::write(const int severity, const std::string& name_tag, const std::string& msg) {
    std::string timestamp = makeTimestamp();
    
    switch (severity) {
        case log_severity::INFO:
            write_to_file(timestamp, INFO_STR, name_tag, msg);
            break;
        case log_severity::WARN:
            write_to_file(timestamp, WARN_STR, name_tag, msg);
            break;
        case log_severity::ERROR:
            write_to_file(timestamp, ERROR_STR, name_tag, msg);
            break;
        default:
            break;
    }

    if (severity >= log_to_console_severity) {
        switch (severity) {
            case log_severity::INFO:
               write_to_console(WHITE_MOD, timestamp, INFO_STR, name_tag, msg);
               break;
           case log_severity::WARN:
              write_to_console(YELLOW_MOD, timestamp, WARN_STR, name_tag, msg);
                break;
          case log_severity::ERROR:
              write_to_console(RED_MOD, timestamp, ERROR_STR, name_tag, msg);
                break;
           default:
                break;
        }
    }
    if (severity >= log_to_connection_severity) {
        write_to_connection(severity, name_tag, msg);
    }
}

void WorkerLogger::write_to_file(const std::string& timestamp, const std::string& severity_tag, const std::string& name_tag, const std::string& msg) {
    if (log_file) {
        (*log_file) << timestamp << " [" << severity_tag << "] [" << name_tag << "] " << msg << std::endl;
    }
}

void WorkerLogger::write_to_console(const std::string& color, const std::string& timestamp, const std::string& severity_tag, const std::string& name_tag, const std::string& msg) {
    std::cout << color << timestamp << " [" << severity_tag << "] [" << name_tag << "] " << msg << RESET_MOD << std::endl; 
}

void WorkerLogger::write_to_connection(const int severity, const std::string& name_tag, const std::string& msg) {
    if (logback) {
        switch (severity) {
            case log_severity::INFO:
                logback(worker::LogLevel::kInfo, name_tag, msg);
                break;
           case log_severity::WARN:
                logback(worker::LogLevel::kWarn, name_tag, msg);
                break;
          case log_severity::ERROR:
                logback(worker::LogLevel::kError, name_tag, msg);
                break;
           default:
                break;
        }
    }
}

void WorkerLogger::init_log_file(const std::string& filename) {
    if (WorkerLogger::log_file) {
        WorkerLogger::write(log_severity::ERROR, "logging", "Tried to initialise log file twice.");
    } else {
        std::ofstream* out = memnew(std::ofstream(filename));
        WorkerLogger::log_file = out;
        (*out) << "--GODOT WORKER LOGFILE START--" << std::endl;
    }
}

void WorkerLogger::init_remote_log_callback(std::function<void(const worker::LogLevel, const std::string&, const std::string&)> callback, log_severity min_sev) {
    if (logback) {
        WorkerLogger::write(log_severity::ERROR, "logging", "Tried to initialise connection logging twice.");
    } else {
        logback = callback;
        log_to_connection_severity = min_sev;
    }
}

void WorkerLogger::on_connection_closed() {
    logback = NULL;
    log_to_connection_severity = log_severity::MAX;
}

void WorkerLogger::set_console_severity(const int console_severity) {
    log_to_connection_severity = console_severity;
}

WorkerLogger::WorkerLogger(const std::string name): logger_name(std::move(name)) {}

WorkerLogger::~WorkerLogger() {
    if (WorkerLogger::log_file) {
        WorkerLogger::log_file->close();
    }
}
