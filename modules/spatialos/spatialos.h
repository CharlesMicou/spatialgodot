#ifndef SPATIALOS_H
#define SPATIALOS_H

#include "editor_node.h"
#include "core/ustring.h"
#include <improbable/worker.h>

class Spatialos : public Node {
    GDCLASS(Spatialos, Node);
    int workerId;
    std::unique_ptr<worker::Dispatcher> dispatcher;
    std::unique_ptr<worker::Connection> connection;
    bool isConnected;

protected:
    static void _bind_methods();

public:
    void joinGame();
    void processOps();
    void setPosition(std::int64_t entityId, double x, double y);
    void sendInfoMessage(const String &msg);

    Spatialos();
};

#endif