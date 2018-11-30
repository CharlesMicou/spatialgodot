#include "editor_node.h"
#include "worker_logger.h"
#include "schema_parser.h"
#include "spatial_util.h"

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

Dictionary SchemaParser::parseComponent(const improbable::MetadataData& data) {
    Dictionary d;
    d["entity_type"] = toGodotString(data.entity_type());
    return d;
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

Dictionary SchemaParser::parseComponent(const spellcrest::PlayerControlsData& data) {
    Dictionary d;
    d["move_destination"] = parseType(data.move_destination());
    return d;
}
