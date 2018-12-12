#ifndef SPATIALOS_H
#define SPATIALOS_H

#include "editor_node.h"
#include "core/ustring.h"
#include <improbable/worker.h>
#include "world_view.h"
#include "worker_logger.h"

class Spatialos : public Node {
    GDCLASS(Spatialos, Node);
    String workerId;
    String workerType;
    WorkerLogger logger;
    std::unique_ptr<worker::Dispatcher> dispatcher;
    std::unique_ptr<worker::Connection> connection;
    WorldView* world_view;
    bool isConnected;
    void setupDispatcher();
    template <typename Metaclass>
    void setupDispatcherForComponentMetaclass();
    void initLogging();

protected:
    static void _bind_methods();

public:
    void blockingConnectLocator(const String &type, const String &dplName, const String &projectName, const String &loginToken);
    void blockingConnectReceptionist(const String &receptionistIp, const int receptionistPort, const String &id, const String &type);
    void processOps();
    void sendInfoMessage(const String &msg);
    template <typename T>
    void sendComponentUpdate(const worker::EntityId entity_id, const typename T::Update& update);
    worker::RequestId<worker::CreateEntityRequest> sendCreateCommand(const worker::Entity& entity);
    worker::RequestId<worker::DeleteEntityRequest> sendDeleteCommand(worker::EntityId entity_id);

    // a hack until a commander exists
    void spawnPlayerEntity(int entity_id, String player_name);
    void reserveId();

    // A debug method that just calls into whatever
    void debug_method(String arbitrary_input);

    String get_configuration_warning() const;

    Spatialos();
};

#endif