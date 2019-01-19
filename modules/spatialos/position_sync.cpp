#include "position_sync.h"
#include "editor_node.h"
#include "spatial_util.h"
#include <improbable/worker.h>
#include "component_view.h"

WorkerLogger PositionSync::logger = WorkerLogger("position_sync");
const double kMinSyncDistanceSq = 10000;

void PositionSync::sync() {
    if (parent == NULL) {
        logger.warn("PositionSync node has no parent, unable to sync.");
        return;
    }
    if (improbable_position_component == NULL || godot_position_component == NULL) {
        logger.warn("PositionSync is missing components, unable to sync.");
        return;
    }
    if (godot_position_component->hasAuthority()) {
        float x = parent->get_position().x;
        float y = parent->get_position().y;
        godotcore::GodotCoordinates2D posToSync = godotcore::GodotCoordinates2D(godotcore::GodotCoordinates2D(godotcore::GodotChunk2D(), godotcore::GodotVector2D(x, y)));
        const godotcore::GodotCoordinates2D currentSyncedPos = godot_position_component->getData().coordinates();
        if (posToSync != currentSyncedPos) {
            godot_position_component->tryUpdate(godotcore::GodotPosition2D::Update{}.set_coordinates(posToSync));
            maybe_sync_spatialos_position(posToSync);
        }
    } else {
        // CSP works by simply not changing the value and assuming the controller
        // has the same behaviour for client and server.
        bool ignore_upstream_position_this_frame = enable_csp && server_data_is_stale;
        if(!ignore_upstream_position_this_frame) {
            std::pair<float, float> local_positon = toLocalGodotPosition(godot_position_component->getData().coordinates(), 0, 0);
            parent->set_position(Vector2(local_positon.first, local_positon.second));
            server_data_is_stale = true;
        }
    }
}

void PositionSync::set_position_components(Node* improbable, Node* godot) {
    improbable_position_component = dynamic_cast<ComponentView<improbable::Position>*>(improbable);
    godot_position_component = dynamic_cast<ComponentView<godotcore::GodotPosition2D>*>(godot);
    if (improbable_position_component == NULL || godot_position_component == NULL) {
        logger.warn("A position sync node received incorrectly configured component refs. "
            "The call order is <improbable position>, <godot position>.");
    }
    parent = dynamic_cast<Node2D*>(get_parent());
    std::pair<float, float> local_positon = toLocalGodotPosition(godot_position_component->getData().coordinates(), 0, 0);
    parent->set_position(Vector2(local_positon.first, local_positon.second));

    // If CSP is enabled, the component node needs connecting to this node so that a signal
    // can be used to indicate the server has fresh corrections
    if(enable_csp) {
        godot->connect("component_updated", this, "notify_update");
    }
}

void PositionSync::maybe_sync_spatialos_position(const godotcore::GodotCoordinates2D& truePos) {
    if (!improbable_position_component->hasAuthority()) {
        return;
    }
    // We don't want to sync improbable position every frame, just when we're sufficiently out of date
    const improbable::PositionData currentSposPos = improbable_position_component->getData();
    godotcore::GodotCoordinates2D sposAsGodot = toGodotPosition(currentSposPos);
    std::pair<float, float> sposLocal = toLocalGodotPosition(sposAsGodot, 0, 0);
    std::pair<float, float> trueLocal = toLocalGodotPosition(truePos, 0, 0);
    double dist_squared = (sposLocal.first - trueLocal.first) * (sposLocal.first - trueLocal.first) + (sposLocal.second - trueLocal.second) * (sposLocal.second - trueLocal.second);
    if (dist_squared > kMinSyncDistanceSq) {
        improbable_position_component->tryUpdate(improbable::Position::Update{}.set_coords(fromGodotPosition(truePos)));
    }
}

void PositionSync::notify_update() {
    server_data_is_stale = false;
}

PositionSync::PositionSync() {
    server_data_is_stale = false;
}

bool PositionSync::get_enable_csp() {
    return enable_csp;
}

void PositionSync::set_enable_csp(bool value) {
    enable_csp = value;
}

void PositionSync::_bind_methods() {
    ClassDB::bind_method(D_METHOD("sync"), &PositionSync::sync);
    ClassDB::bind_method(D_METHOD("notify_update"), &PositionSync::notify_update);
    ClassDB::bind_method(D_METHOD("set_position_components"), &PositionSync::set_position_components);

    ClassDB::bind_method(D_METHOD("set_enable_client_side_prediction", "value"), &PositionSync::set_enable_csp);
	ClassDB::bind_method(D_METHOD("get_enable_client_side_prediction"), &PositionSync::get_enable_csp);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enable_client_side_prediction"), "set_enable_client_side_prediction", "get_enable_client_side_prediction");
}
