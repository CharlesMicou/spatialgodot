#include "editor_node.h"
#include "worker_logger.h"
#include "schema_parser.h"
#include "spatial_util.h"
#include <list>

#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <godotcore/auto_instantiable.h>
#include <godotcore/tile_map_chunk.h>
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

Dictionary SchemaParser::parseType(const improbable::WorkerAttributeSet& data) {
    Dictionary d;
    Array a;
    for (auto v : data.attribute()) {
        a.push_back(toGodotString(v));
    }
    d["attribute"] = a;
    return d;
}

Dictionary SchemaParser::parseType(const improbable::WorkerRequirementSet& data) {
    Dictionary d;
    Array a;
    for (auto v : data.attribute_set()) {
        a.push_back(parseType(v));
    }
    d["attribute_set"] = a;
    return d;
}

Dictionary SchemaParser::parseComponent(const improbable::EntityAclData& data) {
    Dictionary d;
    d["read_acl"] = parseType(data.read_acl());

    Dictionary e1;
    for (auto it : data.component_write_acl()) {
        e1[it.first] = parseType(it.second);
    }
    d["component_write_acl"] = e1;

    data.component_write_acl();
    return d;
}

std::list<Dictionary> SchemaParser::extractEvents(const improbable::EntityAcl::Update& update) {
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

Dictionary SchemaParser::parseComponent(const godotcore::AutoInstantiableData& data) {
    Dictionary d;
    d["scene_name"] = toGodotString(data.scene_name());
    return d;
}

std::list<Dictionary> SchemaParser::extractEvents(const godotcore::AutoInstantiable::Update& update) {
    return std::list<Dictionary>(0);
}

Dictionary SchemaParser::parseType(const godotcore::TileCoordinate& data) {
    Dictionary d;
    d["x"] = data.x();
    d["y"] = data.y();
    return d;
}

Dictionary SchemaParser::parseComponent(const godotcore::TileMapChunkData& data) {
    Dictionary d;
    
    Dictionary e1;
    for (auto it : data.tile_ids()) {
        e1[parseType(it.first)] = it.second;
    }
    d["tile_ids"] = e1;

    return d;
}

std::list<Dictionary> SchemaParser::extractEvents(const godotcore::TileMapChunk::Update& update) {
    return std::list<Dictionary>(0);
}

Dictionary SchemaParser::parseType(const spellcrest::HeartBeat& data) {
    Dictionary d;
    d["timestamp"] = data.timestamp();
    return d;
}

Dictionary SchemaParser::parseComponent(const spellcrest::PlayerControlsData& data) {
    Dictionary d;
    d["move_destination"] = parseType(data.move_destination());
    return d;
}

std::list<Dictionary> SchemaParser::extractEvents(const spellcrest::PlayerControls::Update& update) {
    std::list<Dictionary> l;
    for (auto it : update.spell_cast()) {
        Dictionary d;
        d["spell_cast"] = parseType(it);
        l.push_back(d);
    }
    for (auto it : update.heart_beat()) {
        Dictionary d;
        d["heart_beat"] = parseType(it);
        l.push_back(d);
    }
    return l;
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

Dictionary SchemaParser::parseType(const spellcrest::Health& data) {
    Dictionary d;
    d["max_health"] = data.max_health();
    d["current_health"] = data.current_health();
    return d;
}

Dictionary SchemaParser::parseType(const spellcrest::SpellCast& data) {
    Dictionary d;
    d["spell_id"] = data.spell_id();
    d["cast_target"] = parseType(data.cast_target());
    return d;
}

Dictionary SchemaParser::parseComponent(const spellcrest::MobaUnitData& data) {
    Dictionary d;
    d["health"] = parseType(data.health());
    d["unit_name"] = toGodotString(data.unit_name());
    return d;
}

std::list<Dictionary> SchemaParser::extractEvents(const spellcrest::MobaUnit::Update& update) {
    std::list<Dictionary> l;
    for (auto it : update.spell_cast()) {
        Dictionary d;
        d["spell_cast"] = parseType(it);
        l.push_back(d);
    }
    return l;
}


/* From GDScript to Spatialos */
void SchemaParser::serializeType(improbable::Coordinates& result, const Dictionary d) {
    if (d.has("x")) {
        result.set_x((double) d["x"]);
    }
    if (d.has("y")) {
        result.set_y((double) d["y"]);
    }
    if (d.has("z")) {
        result.set_z((double) d["z"]);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "x" && b != "y" && b != "z") {
            logger.warn("improbable.Coordinates has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeComponentUpdate(improbable::Position::Update& result, const Dictionary d) {
    if (d.has("coords")) {
        improbable::Coordinates v;
        serializeType(v, d["coords"]);
        result.set_coords(v);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "coords") {
            logger.warn("improbable.Position has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeComponentUpdate(improbable::Metadata::Update& result, const Dictionary d) {
    if (d.has("entity_type")) {
        result.set_entity_type(fromGodotString(d["entity_type"]));
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "entity_type") {
            logger.warn("improbable.Metadata has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeComponentUpdate(improbable::EntityAcl::Update &result, const Dictionary d) {
	logger.warn("Attempting to serialize an update for the improbable.EntityAcl component. "
				"This will be ignored, because GDScript should not be directly modifying ACLs!");
}

void SchemaParser::serializeType(godotcore::GodotChunk2D& result, const Dictionary d) {
    if (d.has("x")) {
        result.set_x((int32_t) d["x"]);
    }
    if (d.has("y")) {
        result.set_y((int32_t) d["y"]);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "x" && b != "y") {
            logger.warn("godotcore.GodotChunk2D has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeType(godotcore::GodotVector2D& result, const Dictionary d) {
    if (d.has("x")) {
        result.set_x((float) d["x"]);
    }
    if (d.has("y")) {
        result.set_y((float) d["y"]);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "x" && b != "y") {
            logger.warn("godotcore.GodotVector2D has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeType(godotcore::GodotCoordinates2D& result, const Dictionary d) {
    if (d.has("global_chunk")) {
        godotcore::GodotChunk2D v;
        serializeType(v, d["global_chunk"]);
        result.set_global_chunk(v);
    }
    if (d.has("local_position")) {
        godotcore::GodotVector2D v;
        serializeType(v, d["local_position"]);
        result.set_local_position(v);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "global_chunk" && b != "local_position") {
            logger.warn("godotcore.GodotCoordinates2D has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeComponentUpdate(godotcore::GodotPosition2D::Update& result, const Dictionary d) {
    if (d.has("coordinates")) {
        godotcore::GodotCoordinates2D v;
        serializeType(v, d["coordinates"]);
        result.set_coordinates(v);
    }
    if (d.has("velocity")) {
        godotcore::GodotVector2D v;
        serializeType(v, d["velocity"]);
        result.set_velocity(v);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "coordinates" && b != "velocity") {
            logger.warn("godotcore.GodotPosition2D has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeComponentUpdate(godotcore::AutoInstantiable::Update& result, const Dictionary d) {
    if (d.has("scene_name")) {
        result.set_scene_name(fromGodotString(d["scene_name"]));
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "scene_name") {
            logger.warn("godotcore.AutoInstantiable has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeType(godotcore::TileCoordinate& result, const Dictionary d) {
    if (d.has("x")) {
        result.set_x(d["x"]);
    }
    if (d.has("y")) {
        result.set_y(d["y"]);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "x" && b != "y") {
            logger.warn("godotcore.TileCoordinate has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeComponentUpdate(godotcore::TileMapChunk::Update& result, const Dictionary d) {
    // Note(charlie): the following is totally untested.
    // If you're using this as reference for code-gen, maybe validate that it actually works.
    if (d.has("tile_ids")) {
        worker::Map<godotcore::TileCoordinate, int32_t> m1;
        Dictionary s = d["tile_ids"];
        Array k = s.keys();
        for (int i = 0; i < k.size(); i++) {
            godotcore::TileCoordinate k1;
            serializeType(k1, k.get(i));
            int32_t v1;
            v1 = s[k.get(i)];
            m1.insert({{k1, v1}});
        }
        result.set_tile_ids(m1);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "tile_ids") {
            logger.warn("godotcore.TileMapChunk has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeType(spellcrest::HeartBeat& result, const Dictionary d) {
    if (d.has("timestamp")) {
        result.set_timestamp(d["timestamp"]);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "timestamp") {
            logger.warn("spellcrest.HeartBeat has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeComponentUpdate(spellcrest::PlayerControls::Update& result, const Dictionary d) {
    if (d.has("move_destination")) {
        godotcore::GodotCoordinates2D v;
        serializeType(v, d["move_destination"]);
        result.set_move_destination(v);
    }

    if (d.has("spell_cast")) {
        spellcrest::SpellCast v;
        serializeType(v, d["spell_cast"]);
        result.add_spell_cast(v);
    }

    if (d.has("heart_beat")) {
        spellcrest::HeartBeat v;
        serializeType(v, d["heart_beat"]);
        result.add_heart_beat(v);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "move_destination" && b != "spell_cast" && b!= "heart_beat") {
            logger.warn("spellcrest.PlayerControls has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeType(spellcrest::ChatMessage& result, const Dictionary d) {
    if (d.has("message")) {
        result.set_message(fromGodotString(d["message"]));
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "message") {
            logger.warn("spellcrest.ChatMessage has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeComponentUpdate(spellcrest::ChatParticipant::Update& result, const Dictionary d) {
    if (d.has("chat_name")) {
        result.set_chat_name(fromGodotString(d["chat_name"]));
    }
    
    if (d.has("sent_chat_message")) {
        spellcrest::ChatMessage v;
        serializeType(v, d["sent_chat_message"]);
        result.add_sent_chat_message(v);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "chat_name" && b != "sent_chat_message") {
            logger.warn("spellcrest.ChatParticipant has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeType(spellcrest::Health& result, const Dictionary d) {
    if (d.has("max_health")) {
        result.set_max_health(d["max_health"]);
    }
    if (d.has("current_health")) {
        result.set_current_health(d["current_health"]);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "max_health" && b != "current_health") {
            logger.warn("spellcrest.Health has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeType(spellcrest::SpellCast& result, const Dictionary d) {
    if (d.has("cast_target")) {
        godotcore::GodotCoordinates2D v;
        serializeType(v, d["cast_target"]);
        result.set_cast_target(v);
    }
    if (d.has("spell_id")) {
        result.set_spell_id(d["spell_id"]);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "spell_id" && b != "cast_target") {
            logger.warn("spellcrest.SpellCast has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

void SchemaParser::serializeComponentUpdate(spellcrest::MobaUnit::Update& result, const Dictionary d) {
    if (d.has("unit_name")) {
        result.set_unit_name(fromGodotString(d["unit_name"]));
    }

    if (d.has("health")) {
        spellcrest::Health v;
        serializeType(v, d["health"]);
        result.set_health(v);
    }
    
    if (d.has("spell_cast")) {
        spellcrest::SpellCast v;
        serializeType(v, d["spell_cast"]);
        result.add_spell_cast(v);
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        String b = a.get(i);
        if (b != "unit_name" && b != "spell_cast" && b != "health") {
            logger.warn("spellcrest.MobaUnit has no field " + fromGodotString(b) + ". Ignoring.");
        }
    }
}

/* 
Todo: this implementation can be made less ridiculous by having a templated serializer class
with a type erased base class. For example.

class Base {
    virtual addComponentToEntity(worker::Entity& entity, const Dictionary d)
}

template <typename T>
class SpecificToComponent : public Base {
    actual implementation of addComponentToEntity
}

std::map<worker::ComponentId, Base> serializerLookup;
for each (dictionary entry) -> convert key to component id -> call appropriate serializer

*/
/*
Todo: This currently explodes if supplied empty data (apparently ToInitialData is not valid on empty)
*/
void SchemaParser::applyComponentsToEntity(worker::Entity& entity, const Dictionary d) {
    if (d.has("improbable.Position")) {
        improbable::Position::Update v;
        SchemaParser::serializeComponentUpdate(v, d["improbable.Position"]);
        entity.Add<improbable::Position>(v.ToInitialData());
    }
    if (d.has("improbable.Metadata")) {
        improbable::Metadata::Update v;
        SchemaParser::serializeComponentUpdate(v, d["improbable.Metadata"]);
        entity.Add<improbable::Metadata>(v.ToInitialData());
    }
    if (d.has("godotcore.GodotPosition2D")) {
        godotcore::GodotPosition2D::Update v;
        SchemaParser::serializeComponentUpdate(v, d["godotcore.GodotPosition2D"]);
        entity.Add<godotcore::GodotPosition2D>(v.ToInitialData());
    }
    if (d.has("godotcore.AutoInstantiable")) {
        godotcore::AutoInstantiable::Update v;
        SchemaParser::serializeComponentUpdate(v, d["godotcore.AutoInstantiable"]);
        entity.Add<godotcore::AutoInstantiable>(v.ToInitialData());
    }
    if (d.has("godotcore.TileMapChunk")) {
        godotcore::TileMapChunk::Update v;
        SchemaParser::serializeComponentUpdate(v, d["godotcore.TileMapChunk"]);
        entity.Add<godotcore::TileMapChunk>(v.ToInitialData());
    }
    if (d.has("spellcrest.PlayerControls")) {
        spellcrest::PlayerControls::Update v;
        SchemaParser::serializeComponentUpdate(v, d["spellcrest.PlayerControls"]);
        entity.Add<spellcrest::PlayerControls>(v.ToInitialData());
    }
    if (d.has("spellcrest.ChatParticipant")) {
        spellcrest::ChatParticipant::Update v;
        SchemaParser::serializeComponentUpdate(v, d["spellcrest.ChatParticipant"]);
        entity.Add<spellcrest::ChatParticipant>(v.ToInitialData());
    }
    if (d.has("spellcrest.MobaUnit")) {
        spellcrest::MobaUnit::Update v;
        SchemaParser::serializeComponentUpdate(v, d["spellcrest.MobaUnit"]);
        entity.Add<spellcrest::MobaUnit>(v.ToInitialData());
    }

    Array a = d.keys();
    for (int i = 0; i < a.size(); i++) {
        std::string b = fromGodotString(a.get(i));
        auto it = schema_component_ids.find(b);
        if (it == schema_component_ids.end()) {
            logger.warn(b + " is not a known component and cannot be added to an entity. Ignoring.");
        }
    }
}

