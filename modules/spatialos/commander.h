#ifndef SPATIALOS_COMMANDER_H
#define SPATIALOS_COMMANDER_H

#include "editor_node.h"
#include "worker_logger.h"
#include <improbable/worker.h>

class Commander : public Node {
    GDCLASS(Commander, Node);
    static WorkerLogger logger;
    worker::Map<worker::RequestId<worker::CreateEntityRequest>, bool> pending_creates;
    worker::Map<worker::RequestId<worker::DeleteEntityRequest>, bool> pending_deletes;
    Node* spos;

protected:
    static void _bind_methods() {
            ADD_SIGNAL(MethodInfo("authority_changed", PropertyInfo(Variant::BOOL, "authority")));
            ADD_SIGNAL(MethodInfo("component_event", PropertyInfo(Variant::DICTIONARY, "event")));
            ClassDB::bind_method(D_METHOD("create_entity", "component_data", "local_position", "specific_worker", "persistant"), &Commander::create_entity);
            ClassDB::bind_method(D_METHOD("delete_entity", "entity_id"), &Commander::delete_entity);
    }

public:
    /* These return command ids that can be used in callbacks, not entity ids. */
    int create_entity(Dictionary components, Vector2 local_position, String specific_worker, bool persistant);
    int delete_entity(std::int64_t entity_id);

    // Default args for common cases
    int create_entity(Dictionary components, Vector2 local_position, String specific_worker, bool persistant = true);
    int create_entity(Dictionary components, Vector2 local_position, String specific_worker = "", bool persistant = true);

    void handle_create_response();
    void handle_delete_response();

    void set_spos_node(Node* connection);

    Commander();
};

#endif