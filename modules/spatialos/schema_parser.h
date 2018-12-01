#ifndef SPATIALOS_SCHEMA_PARSER_H
#define SPATIALOS_SCHEMA_PARSER_H

#include "editor_node.h"
#include "worker_logger.h"
#include <list>

#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <spellcrest/player_controls.h>

const std::unordered_map<worker::ComponentId, std::string> schema_component_names {
            {53, "improbable::Metadata"},
            {54, "improbable::Position"},
            {3001, "godotcore::GodotPosition2D"},
            {4001, "spellcrest::PlayerControls"},
            {4002, "spellcrest:ChatParticipant"}
};

const std::unordered_map<std::string, worker::ComponentId> schema_component_ids {
            {"improbable::Metadata", 53},
            {"improbable::Position", 54},
            {"godotcore::GodotPosition2D", 3001},
            {"spellcrest::PlayerControls", 4001},
            {"spellcrest:ChatParticipant", 4002}
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

        static Dictionary parseComponent(const spellcrest::PlayerControlsData& data);
        static std::list<Dictionary> extractEvents(const spellcrest::PlayerControls::Update& update);

        static Dictionary parseType(const spellcrest::ChatMessage& data);
        static Dictionary parseComponent(const spellcrest::ChatParticipantData& data);
        static std::list<Dictionary> extractEvents(const spellcrest::ChatParticipant::Update& update);

        // Conversions to Schema format from Godot format
        // Force passing in dummy pointers so that we can overload properly
        static const improbable::Coordinates serializeType(improbable::Coordinates* dummy, const Dictionary d);
        static const improbable::Position::Update serializeComponentUpdate(improbable::Position::Update* dummy, const Dictionary d);

        static const improbable::Metadata::Update serializeComponentUpdate(improbable::Metadata::Update* dummy, const Dictionary d);

        static const godotcore::GodotChunk2D serializeType(godotcore::GodotChunk2D* dummy, const Dictionary d);
        static const godotcore::GodotCoordinates2D serializeType(godotcore::GodotCoordinates2D* dummy, const Dictionary d);
        static const godotcore::GodotVector2D serializeType(godotcore::GodotVector2D* dummy, const Dictionary d);
        static const godotcore::GodotPosition2D::Update serializeComponentUpdate(godotcore::GodotPosition2DData* dummy, const Dictionary d);

        static const spellcrest::PlayerControls::Update serializeComponentUpdate(spellcrest::PlayerControls::Update* dummy, const Dictionary d);

        static const spellcrest::ChatMessage serializeType(spellcrest::ChatMessage* dummy, const Dictionary d);
        static const spellcrest::ChatParticipant::Update serializeComponentUpdate(spellcrest::ChatParticipant::Update* dummy, const Dictionary d);
};

#endif