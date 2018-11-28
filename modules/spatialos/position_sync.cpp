#include "position_sync.h"
#include "editor_node.h"
#include "spatial_util.h"

WorkerLogger PositionSync::logger = WorkerLogger("position_sync");

void PositionSync::sync() {
    parent = dynamic_cast<Node2D*>(get_parent());
    if (parent == NULL) {
        logger.warn("PositionSync node has no parent, unable to sync.");
        return;
    }
    if (position_component == NULL) {
        return;
    }
    if (position_component->hasAuthority()) {
        float x = parent->get_position().x;
        float y = parent->get_position().y;
        godotcore::GodotCoordinates2D asGodotData = godotcore::GodotCoordinates2D(godotcore::GodotCoordinates2D(godotcore::GodotChunk2D(), godotcore::GodotVector2D(x, y)));
        // Only update the position if it has actually changed
        // Disabling this for now because for some reason it causes a ton of jitter
        // Suspect this will be fixed by not storing the position internally to here but reffing the component view instead
        //if (last_position != asGodotData) {
        if (true) {
            // still need to look at this
            // last_position = asGodotData;
            position_component->tryUpdate(improbable::Position::Update{}.set_coords(fromGodotPosition(asGodotData)));
        }
        
    } else {
        const improbable::Coordinates& coords = position_component->getData().coords();
        godotcore::GodotCoordinates2D global = toGodotPosition(coords);
        std::pair<float, float> local_positon = toLocalGodotPosition(global, 0, 0);
        parent->set_position(Vector2(local_positon.first, local_positon.second));
    }
}

void PositionSync::set_position(Node* ref) {
    position_component = dynamic_cast<ComponentView<improbable::Position>*>(ref);
}

PositionSync::PositionSync() {
}

void PositionSync::_bind_methods() {
    ClassDB::bind_method(D_METHOD("sync"), &PositionSync::sync);
    ClassDB::bind_method(D_METHOD("set_position_component"), &PositionSync::set_position);
}
