#ifndef PLAYER_CONTROLS_SYNC_H
#define PLAYER_CONTROLS_SYNC_H

#include "editor_node.h"
#include "worker_logger.h"
#include "component_view.h"
#include <spellcrest/player_controls.h>

class PlayerControlsSync : public Node {
    GDCLASS(PlayerControlsSync, Node);
    static WorkerLogger logger;

    ComponentView<spellcrest::PlayerControls>* controls_component;

protected:
    static void _bind_methods();

public:
    PlayerControlsSync();

    // This currently returns the current value of the component because
    // I haven't implemented component accessors for views yet
    Vector2 get_controls_value();

    void sync(const Vector2 destination);

    void set_player_controls_component(Node* component);
};

#endif