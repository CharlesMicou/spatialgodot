#ifndef WORKER_LOGGER_WRAPPER_H
#define WORKER_LOGGER_WRAPPER_H

#include "editor_node.h"
#include "worker_logger.h"

class WorkerLoggerWrapper : public Node {
    private:
        GDCLASS(WorkerLoggerWrapper, Node);
        static WorkerLogger logger;

    protected:
        static void _bind_methods();

    public:
        // todo: dynamic binds so that gdscript context is propagated
        void info(const String& msg);
        void warn(const String& msg);
        void error(const String& msg);

        WorkerLoggerWrapper();
};

#endif