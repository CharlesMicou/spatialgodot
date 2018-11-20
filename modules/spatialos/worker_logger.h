#include <iostream>
#include <fstream>
#include <improbable/worker.h>

enum log_severity {
    INFO = 0,
    WARN = 1,
    ERROR = 2,
    MAX = 3
};

class WorkerLogger {
    private:
        const std::string logger_name;

        static std::ofstream log_file;
        static worker::Connection* connection;

        static int log_to_connection_severity;
        static int log_to_console_severity;

        static void write(const int severity, const std::string& name_tag, const std::string& msg);
        static void write_to_file(const std::string& timestamp, const std::string& severity_tag, const std::string& name_tag, const std::string& msg);
        static void write_to_connection(const int severity, const std::string& name_tag, const std::string& msg);
        static void write_to_console(const std::string& color, const std::string& timestamp, const std::string& severity_tag, const std::string& name_tag, const std::string& msg);

    public:
        void info(const std::string& msg);
        void warn(const std::string& msg);
        void error(const std::string& msg);

        static void init_log_file(const std::string& filename);
        static void init_connection(worker::Connection* upstream, const int min_severity);
        static void on_connection_closed();
        static void set_console_severity(const int console_severity);

        WorkerLogger(const std::string name);
        ~WorkerLogger();
};
