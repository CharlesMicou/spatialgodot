#ifndef POSITION_SYNC_H
#define POSITION_SYNC_H

#include "editor_node.h"
#include "worker_logger.h"
#include "component_view.h"
#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>

class PositionSync : public Node {
    GDCLASS(PositionSync, Node);
    static WorkerLogger logger;

    Node2D* parent;
    bool enable_csp;
    bool server_data_is_stale;
    ComponentView<improbable::Position>* improbable_position_component;
    ComponentView<godotcore::GodotPosition2D>* godot_position_component;

    void maybe_sync_spatialos_position(const godotcore::GodotCoordinates2D& truePos);

protected:
    static void _bind_methods();

public:
    PositionSync();
    void sync();
    void set_position_components(Node* improbable, Node* godot);
    void notify_update();

    bool get_enable_csp();
    void set_enable_csp(bool value);
};

#endif