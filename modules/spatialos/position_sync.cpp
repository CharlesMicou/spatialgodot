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
        // Todo: only send data when the data has changed
        // Todo: only send improbable position when sufficiently far away from last position
        const godotcore::GodotCoordinates2D currentSyncedPos = godot_position_component->getData().coordinates();
        if (posToSync != currentSyncedPos) {
            godot_position_component->tryUpdate(godotcore::GodotPosition2D::Update{}.set_coordinates(posToSync));
            maybe_sync_spatialos_position(posToSync);
        }
    } else {
        std::pair<float, float> local_positon = toLocalGodotPosition(godot_position_component->getData().coordinates(), 0, 0);
        parent->set_position(Vector2(local_positon.first, local_positon.second));
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

PositionSync::PositionSync() {
}

void PositionSync::_bind_methods() {
    ClassDB::bind_method(D_METHOD("sync"), &PositionSync::sync);
    ClassDB::bind_method(D_METHOD("set_position_components"), &PositionSync::set_position_components);
}
