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

/* From GDScript to Spatialos */
const improbable::Coordinates SchemaParser::serializeType(improbable::Coordinates* dummy, const Dictionary d) {
    improbable::Coordinates v;
    if (d.has("x")) {
        v.set_x((double) d["x"]);
    }
    if (d.has("y")) {
        v.set_y((double) d["y"]);
    }
    if (d.has("z")) {
        v.set_z((double) d["z"]);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "x" && b != "y" && b != "z") {
            logger.warn("improbable.Coordinates has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }

    return v;
}

const improbable::Position::Update SchemaParser::serializeComponentUpdate(improbable::Position::Update* dummy, const Dictionary d) {
    improbable::Position::Update u;
    if (d.has("coords")) {
        improbable::Coordinates* dummy1;
        u.set_coords(serializeType(dummy1, d["coords"]));
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "coords") {
            logger.warn("improbable.Position has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }

    return u;
}

const improbable::Metadata::Update SchemaParser::serializeComponentUpdate(improbable::Metadata::Update* dummy, const Dictionary d) {
    improbable::Metadata::Update u;
    if (d.has("entity_type")) {
        u.set_entity_type(fromGodotString(d["entity_type"]));
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "entity_type") {
            logger.warn("improbable.Metadata has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }

    return u;
}

const godotcore::GodotChunk2D SchemaParser::serializeType(godotcore::GodotChunk2D* dummy, const Dictionary d) {
    godotcore::GodotChunk2D v;
    if (d.has("x")) {
        v.set_x((int32_t) d["x"]);
    }
    if (d.has("y")) {
        v.set_y((int32_t) d["y"]);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "x" && b != "y") {
            logger.warn("godotcore.GodotChunk2D has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }

    return v;
}

const godotcore::GodotVector2D SchemaParser::serializeType(godotcore::GodotVector2D* dummy, const Dictionary d) {
    godotcore::GodotVector2D v;
    if (d.has("x")) {
        v.set_x((float) d["x"]);
    }
    if (d.has("y")) {
        v.set_y((float) d["y"]);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "x" && b != "y") {
            logger.warn("godotcore.GodotVector2D has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }

    return v;
}

const godotcore::GodotCoordinates2D SchemaParser::serializeType(godotcore::GodotCoordinates2D* dummy, const Dictionary d) {
    godotcore::GodotCoordinates2D v;
    if (d.has("global_chunk")) {
        godotcore::GodotChunk2D* dummy1;
        v.set_global_chunk(serializeType(dummy1, d["global_chunk"]));
    }
    if (d.has("local_position")) {
        godotcore::GodotVector2D* dummy1;
        v.set_local_position(serializeType(dummy1, d["local_position"]));
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "global_chunk" && b != "local_position") {
            logger.warn("godotcore.GodotCoordinates2D has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }

    return v;
}

const godotcore::GodotPosition2D::Update SchemaParser::serializeComponentUpdate(godotcore::GodotPosition2DData* dummy, const Dictionary d) {
    godotcore::GodotPosition2D::Update u;
    if (d.has("coordinates")) {
        godotcore::GodotCoordinates2D* dummy1;
        u.set_coordinates(serializeType(dummy1, d["coordinates"]));
    }
    if (d.has("velocity")) {
        godotcore::GodotVector2D* dummy1;
        u.set_velocity(serializeType(dummy1, d["velocity"]));
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "coordinates" && b != "velocity") {
            logger.warn("godotcore.GodotPosition2D has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }

    return u;
}

const spellcrest::PlayerControls::Update SchemaParser::serializeComponentUpdate(spellcrest::PlayerControls::Update* dummy, const Dictionary d) {
    spellcrest::PlayerControls::Update u;
    if (d.has("move_destination")) {
        godotcore::GodotCoordinates2D* dummy1;
        u.set_move_destination(serializeType(dummy1, d["move_destination"]));
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "move_destination") {
            logger.warn("spellcrest.PlayerControls has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }

    return u;
}

const spellcrest::ChatMessage SchemaParser::serializeType(spellcrest::ChatMessage* dummy, const Dictionary d) {
    spellcrest::ChatMessage v;
    if (d.has("message")) {
        v.set_message(fromGodotString(d["message"]));
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "message") {
            logger.warn("spellcrest.ChatMessage has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }

    return v;
}

const spellcrest::ChatParticipant::Update SchemaParser::serializeComponentUpdate(spellcrest::ChatParticipant::Update* dummy, const Dictionary d) {
    spellcrest::ChatParticipant::Update u;
    if (d.has("chat_name")) {
        u.set_chat_name(fromGodotString(d["chat_name"]));
    }
    
    if (d.has("sent_chat_message")) {
        spellcrest::ChatMessage* dummy1;
        u.add_sent_chat_message(serializeType(dummy1, d["sent_chat_message"]));
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "chat_name" && b != "sent_chat_message") {
            logger.warn("spellcrest.ChatParticipant has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }

    return u;
}