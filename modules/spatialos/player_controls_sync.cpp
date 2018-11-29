#include "player_controls_sync.h"
#include "editor_node.h"
#include "spatial_util.h"
#include <improbable/worker.h>
#include "component_view.h"
#include <spellcrest/player_controls.h>

WorkerLogger PlayerControlsSync::logger = WorkerLogger("position_sync");

void PlayerControlsSync::sync(const Vector2 destination) {
    parent = dynamic_cast<Node2D*>(get_parent());
    if (parent == NULL) {
        logger.warn("PlayerControlsSync node has no parent, unable to sync.");
        return;
    }
    if (controls_component == NULL) {
        return;
    }
    if (controls_component->hasAuthority()) {
        float x = destination.x;
        float y = destination.y;
        godotcore::GodotCoordinates2D asGodotData = godotcore::GodotCoordinates2D(godotcore::GodotCoordinates2D(godotcore::GodotChunk2D(), godotcore::GodotVector2D(x, y)));
        controls_component->tryUpdate(spellcrest::PlayerControls::Update{}.set_move_destination(asGodotData));
    }
}

Vector2 PlayerControlsSync::get_controls_value() {
    if (controls_component == NULL) {
        return Vector2();
    }
    std::pair<float, float> localpos = toLocalGodotPosition(controls_component->getData().move_destination(), 0, 0);
    return Vector2(localpos.first, localpos.second);
}

void PlayerControlsSync::set_player_controls_component(Node* component) {
    controls_component = dynamic_cast<ComponentView<spellcrest::PlayerControls>*>(component);
    if (controls_component == NULL) {
        logger.warn("A PlayerControlsSync node received incorrectly configured component refs.");
    }
}

PlayerControlsSync::PlayerControlsSync() {
}

void PlayerControlsSync::_bind_methods() {
    ClassDB::bind_method(D_METHOD("sync"), &PlayerControlsSync::sync);
    ClassDB::bind_method(D_METHOD("set_player_controls_components"), &PlayerControlsSync::set_player_controls_component);
}
