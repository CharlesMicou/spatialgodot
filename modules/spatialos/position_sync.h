#ifndef POSITION_SYNC_H
#define POSITION_SYNC_H

#include "editor_node.h"
#include "spatialos.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>

class PositionSync : public Node {
    GDCLASS(PositionSync, Node);
    static WorkerLogger logger;

    worker::EntityId entity_id;
    Spatialos* connection;
    Node2D* parent;
    ComponentView<improbable::Position>* position_component;

protected:
    static void _bind_methods();

public:
    PositionSync();
    void sync();
    void set_entity_id(std::int64_t id);
    void set_connection(Node* spos);
    void set_position(Node* ref);
};

#endif