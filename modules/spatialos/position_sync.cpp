#include "position_sync.h"
#include "editor_node.h"
#include "spatial_util.h"
#include <iostream>
#include <stdio.h>

void PositionSync::sync() {
    // todo fix this
    parent = dynamic_cast<Node2D*>(get_parent());
    if (parent == NULL) {
        std::cout << "PARENT WAS NULL" << std::endl;
    }
    if (connection == NULL || position_component == NULL) {
        std::cout << "Failed to sync position because a ref was not set" << std::endl;
        return;
    }
    if (!position_component->hasAuthority()) {
        std::cout << "Didn't try to sync position because no authority" << std::endl;
        return;
    }
    float x = parent->get_position().x;
    float y = parent->get_position().y;
    godotcore::GodotPosition2DData asGodotData = godotcore::GodotPosition2DData(godotcore::GodotChunk2D(), godotcore::GodotVector2D(x, y), godotcore::GodotVector2D());
    printf("Trying to sync godot cords %f %f for entity %ld \n", x ,y, entity_id);
    connection->sendComponentUpdate<improbable::Position>(entity_id, improbable::Position::Update{}.set_coords(fromGodotPosition(asGodotData)));
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
