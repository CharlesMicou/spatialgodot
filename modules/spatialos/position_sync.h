#ifndef POSITION_SYNC_H
#define POSITION_SYNC_H

#include "editor_node.h"
#include "node_2d.h"
#include "spatialos.h"
#include <improbable/worker.h>
#include <godotcore/godot_position2d.h>

class PositionSync : public Node {
    GDCLASS(PositionSync, Node);
    Spatialos& connection;
    Node2D& sync_node;
    worker::EntityId entity_id;
    void sync();

protected:
    static void _bind_methods();

public:
    godotcore::GodotPosition2D current_position;
    PositionSync();
};

#endif