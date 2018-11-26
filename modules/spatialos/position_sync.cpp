#include "position_sync.h"
#include "editor_node.h"
#include "spatial_util.h"
#include <stdio.h>

WorkerLogger PositionSync::logger = WorkerLogger("position_sync");

void PositionSync::sync() {
    // todo fix this
    parent = dynamic_cast<Node2D*>(get_parent());
    if (parent == NULL) {
        logger.warn("Parent was null");
        return;
    }
    if (connection == NULL || position_component == NULL) {
        return;
    }
    if (position_component->hasAuthority()) {
        float x = parent->get_position().x;
        float y = parent->get_position().y;
        godotcore::GodotPosition2DData asGodotData = godotcore::GodotPosition2DData(godotcore::GodotChunk2D(), godotcore::GodotVector2D(x, y), godotcore::GodotVector2D());
        // Only update the position if it has actually changed
        // Disabling this for now because for some reason it causes a ton of jitter
        //if (last_position != asGodotData) {
        if (true) {
            last_position = asGodotData;
            connection->sendComponentUpdate<improbable::Position>(entity_id, improbable::Position::Update{}.set_coords(fromGodotPosition(asGodotData)));
        }
        
    } else {
        const improbable::Coordinates& coords = position_component->getData().coords();
        godotcore::GodotPosition2DData global = toGodotPosition(coords);
        std::pair<float, float> local_positon = toLocalGodotPosition(global, 0, 0);
        parent->set_position(Vector2(local_positon.first, local_positon.second));
    }
}

void PositionSync::set_connection(Node* spos) {
    connection = dynamic_cast<Spatialos*>(spos);
}

void PositionSync::set_entity_id(std::int64_t id) {
    entity_id = id;
}

void PositionSync::set_position(Node* ref) {
    position_component = dynamic_cast<ComponentView<improbable::Position>*>(ref);
}

PositionSync::PositionSync() {
}

void PositionSync::_bind_methods() {
    ClassDB::bind_method(D_METHOD("sync"), &PositionSync::sync);
    ClassDB::bind_method(D_METHOD("set_connection_node"), &PositionSync::set_connection);
    ClassDB::bind_method(D_METHOD("set_entity_id"), &PositionSync::set_entity_id);
    ClassDB::bind_method(D_METHOD("set_position_component"), &PositionSync::set_position);
}
