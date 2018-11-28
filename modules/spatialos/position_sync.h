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

    Node2D* parent;
    ComponentView<improbable::Position>* improbable_position_component;
    ComponentView<godotcore::GodotPosition2D>* godot_position_component;

protected:
    static void _bind_methods();

public:
    PositionSync();
    void sync();
    void set_position_components(Node* improbable, Node* godot);
};

#endif