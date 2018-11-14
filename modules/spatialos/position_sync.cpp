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
        return;
    }
    if (connection == NULL || position_component == NULL) {
        return;
    }
    if (position_component->hasAuthority()) {
        float x = parent->get_position().x;
        float y = parent->get_position().y;
        godotcore::GodotPosition2DData asGodotData = godotcore::GodotPosition2DData(godotcore::GodotChunk2D(), godotcore::GodotVector2D(x, y), godotcore::GodotVector2D());
        connection->sendComponentUpdate<improbable::Position>(entity_id, improbable::Position::Update{}.set_coords(fromGodotPosition(asGodotData)));
    } else {
        parent->set_position(position_component->getPosition());
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
