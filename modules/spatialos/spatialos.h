#ifndef SPATIALOS_H
#define SPATIALOS_H

#include "editor_node.h"
#include "core/ustring.h"
#include <improbable/worker.h>

class Spatialos : public Node {
    GDCLASS(Spatialos, Node);
    String workerId;
    String workerType;
    std::unique_ptr<worker::Dispatcher> dispatcher;
    std::unique_ptr<worker::Connection> connection;
    bool isConnected;
    void postConnection();

protected:
    static void _bind_methods();

public:
    void blockingConnectLocator(const String &type, const String &dplName, const String &projectName, const String &loginToken);
    void blockingConnectReceptionist(const String &receptionistIp, const int receptionistPort, const String &id, const String &type);
    void processOps();
    void setPosition(std::int64_t entityId, double x, double y);
    void sendInfoMessage(const String &msg);

    Spatialos();
};

#endif