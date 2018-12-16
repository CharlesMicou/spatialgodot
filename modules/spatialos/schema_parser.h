#ifndef SPATIALOS_SCHEMA_PARSER_H
#define SPATIALOS_SCHEMA_PARSER_H

#include "editor_node.h"
#include "worker_logger.h"
#include <list>

#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <godotcore/auto_instantiable.h>
#include <spellcrest/player_controls.h>
#include <spellcrest/moba_unit.h>

const std::unordered_map<worker::ComponentId, std::string> schema_component_names {
            {53, "improbable.Metadata"},
            {54, "improbable.Position"},
            {3001, "godotcore.GodotPosition2D"},
            {3002, "godotcore.AutoInstantiable"},
            {4001, "spellcrest.PlayerControls"},
            {4002, "spellcrest.ChatParticipant"},
            {4101, "spellcrest.MobaUnit"}
};

const std::unordered_map<std::string, worker::ComponentId> schema_component_ids {
            {"improbable.Metadata", 53},
            {"improbable.Position", 54},
            {"godotcore.GodotPosition2D", 3001},
            {"godotcore.AutoInstantiable", 3002},
            {"spellcrest.PlayerControls", 4001},
            {"spellcrest.ChatParticipant", 4002},
            {"spellcrest.MobaUnit", 4101}
};

class SchemaParser {
    private:
        static WorkerLogger logger;

    public:
        // Conversions to Godot format from Schema format
        static Dictionary parseType(const improbable::Coordinates& data);
        static Dictionary parseComponent(const improbable::PositionData& data);
        static std::list<Dictionary> extractEvents(const improbable::Position::Update& update);

        static Dictionary parseComponent(const improbable::MetadataData& data);
        static std::list<Dictionary> extractEvents(const improbable::Metadata::Update& update);

        static Dictionary parseType(const godotcore::GodotChunk2D& data);
        static Dictionary parseType(const godotcore::GodotCoordinates2D& data);
        static Dictionary parseType(const godotcore::GodotVector2D& data);
        static Dictionary parseComponent(const godotcore::GodotPosition2DData& data);
        static std::list<Dictionary> extractEvents(const godotcore::GodotPosition2D::Update& update);

        static Dictionary parseComponent(const godotcore::AutoInstantiableData& data);
        static std::list<Dictionary> extractEvents(const godotcore::AutoInstantiable::Update& update);

        static Dictionary parseType(const spellcrest::HeartBeat& data);
        static Dictionary parseComponent(const spellcrest::PlayerControlsData& data);
        static std::list<Dictionary> extractEvents(const spellcrest::PlayerControls::Update& update);

        static Dictionary parseType(const spellcrest::ChatMessage& data);
        static Dictionary parseComponent(const spellcrest::ChatParticipantData& data);
        static std::list<Dictionary> extractEvents(const spellcrest::ChatParticipant::Update& update);

        static Dictionary parseType(const spellcrest::Health& data);
        static Dictionary parseType(const spellcrest::SpellCast& data);
        static Dictionary parseComponent(const spellcrest::MobaUnitData& data);
        static std::list<Dictionary> extractEvents(const spellcrest::MobaUnit::Update& update);

        // Conversions to Schema format from Godot format
        static void serializeType(improbable::Coordinates& result, const Dictionary d);
        static void serializeComponentUpdate(improbable::Position::Update& result, const Dictionary d);

        static void serializeComponentUpdate(improbable::Metadata::Update& result, const Dictionary d);

        static void serializeType(godotcore::GodotChunk2D& result, const Dictionary d);
        static void serializeType(godotcore::GodotCoordinates2D& result, const Dictionary d);
        static void serializeType(godotcore::GodotVector2D& result, const Dictionary d);
        static void serializeComponentUpdate(godotcore::GodotPosition2D::Update& result, const Dictionary d);

        static void serializeComponentUpdate(godotcore::AutoInstantiable::Update& result, const Dictionary d);

        static void serializeType(spellcrest::HeartBeat& result, const Dictionary d);
        static void serializeComponentUpdate(spellcrest::PlayerControls::Update& result, const Dictionary d);

        static void serializeType(spellcrest::ChatMessage& result, const Dictionary d);
        static void serializeComponentUpdate(spellcrest::ChatParticipant::Update& result, const Dictionary d);

        static void serializeType(spellcrest::Health& result, const Dictionary d);
        static void serializeType(spellcrest::SpellCast& result, const Dictionary d);
        static void serializeComponentUpdate(spellcrest::MobaUnit::Update& result, const Dictionary d);

        // Embarassingly inefficient entity serialization
        static void applyComponentsToEntity(worker::Entity& entity, const Dictionary d);
};

#endif