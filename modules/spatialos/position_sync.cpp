#include "position_sync.h"
#include "editor_node.h"
#include "spatial_util.h"
#include <improbable/worker.h>
#include "component_view.h"

WorkerLogger PositionSync::logger = WorkerLogger("position_sync");

void PositionSync::sync() {
    if (parent == NULL) {
        logger.warn("PositionSync node has no parent, unable to sync.");
        return;
    }
    if (improbable_position_component == NULL || godot_position_component == NULL) {
        logger.warn("PositionSync is missing components, unable to sync.");
        return;
    }
    if (improbable_position_component->hasAuthority() && godot_position_component->hasAuthority()) {
        float x = parent->get_position().x;
        float y = parent->get_position().y;
        godotcore::GodotCoordinates2D asGodotData = godotcore::GodotCoordinates2D(godotcore::GodotCoordinates2D(godotcore::GodotChunk2D(), godotcore::GodotVector2D(x, y)));
        // Todo: only send data when the data has changed
        // Todo: only send improbable position when sufficiently far away from last position
        if (asGodotData != godot_position_component->getData().coordinates) {
            godot_position_component->tryUpdate(godotcore::GodotPosition2D::Update{}.set_coordinates(asGodotData));
        }
        improbable_position_component->tryUpdate(improbable::Position::Update{}.set_coords(fromGodotPosition(asGodotData)));
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

PositionSync::PositionSync() {
}

void PositionSync::_bind_methods() {
    ClassDB::bind_method(D_METHOD("sync"), &PositionSync::sync);
    ClassDB::bind_method(D_METHOD("set_position_components"), &PositionSync::set_position_components);
}
