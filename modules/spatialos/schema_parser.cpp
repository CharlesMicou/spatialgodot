#include "editor_node.h"
#include "worker_logger.h"
#include "schema_parser.h"
#include "spatial_util.h"
#include <list>

#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <spellcrest/player_controls.h>

WorkerLogger SchemaParser::logger = WorkerLogger("schema_parser");

Dictionary SchemaParser::parseType(const improbable::Coordinates& data) {
    Dictionary d;
    d["x"] = data.x();
    d["y"] = data.y();
    d["z"] = data.z();
    return d;
}

Dictionary SchemaParser::parseComponent(const improbable::PositionData& data) {
    Dictionary d;
    d["coords"] = parseType(data.coords());
    return d;
}

std::list<Dictionary> SchemaParser::extractEvents(const improbable::Position::Update& update) {
    return std::list<Dictionary>(0);
}

Dictionary SchemaParser::parseComponent(const improbable::MetadataData& data) {
    Dictionary d;
    d["entity_type"] = toGodotString(data.entity_type());
    return d;
}

std::list<Dictionary> SchemaParser::extractEvents(const improbable::Metadata::Update& update) {
    return std::list<Dictionary>(0);
}

Dictionary SchemaParser::parseType(const godotcore::GodotChunk2D& data) {
    Dictionary d;
    d["x"] = data.x();
    d["y"] = data.y();
    return d;
}

Dictionary SchemaParser::parseType(const godotcore::GodotCoordinates2D& data) {
    Dictionary d;
    d["global_chunk"] = parseType(data.global_chunk());
    d["local_position"] = parseType(data.local_position());
    return d;
}

Dictionary SchemaParser::parseType(const godotcore::GodotVector2D& data) {
    Dictionary d;
    d["x"] = data.x();
    d["y"] = data.y();
    return d;
}

Dictionary SchemaParser::parseComponent(const godotcore::GodotPosition2DData& data) {
    Dictionary d;
    d["coordinates"] = parseType(data.coordinates());
    d["velocity"] = parseType(data.velocity());
    return d;
}

std::list<Dictionary> SchemaParser::extractEvents(const godotcore::GodotPosition2D::Update& update) {
    return std::list<Dictionary>(0);
}

Dictionary SchemaParser::parseComponent(const spellcrest::PlayerControlsData& data) {
    Dictionary d;
    d["move_destination"] = parseType(data.move_destination());
    return d;
}

std::list<Dictionary> SchemaParser::extractEvents(const spellcrest::PlayerControls::Update& update) {
    return std::list<Dictionary>(0);
}

Dictionary SchemaParser::parseType(const spellcrest::ChatMessage& data) {
    Dictionary d;
    d["message"] = toGodotString(data.message());
    return d;
}

Dictionary SchemaParser::parseComponent(const spellcrest::ChatParticipantData& data) {
    Dictionary d;
    d["chat_name"] = toGodotString(data.chat_name());
    return d;
}

std::list<Dictionary> SchemaParser::extractEvents(const spellcrest::ChatParticipant::Update& update) {
    std::list<Dictionary> l;
    for (auto it : update.sent_chat_message()) {
        Dictionary d;
        d["sent_chat_message"] = parseType(it);
        l.push_back(d);
    }
    return l;
}
