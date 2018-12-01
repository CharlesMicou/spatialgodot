#ifndef SPATIALOS_SCHEMA_PARSER_H
#define SPATIALOS_SCHEMA_PARSER_H

#include "editor_node.h"
#include "worker_logger.h"

#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <spellcrest/player_controls.h>

const std::unordered_map<worker::ComponentId, std::string> schema_component_names {
            {53, "improbable::Metadata"},
            {54, "improbable::Position"},
            {3001, "godotcore::GodotPosition2D"},
            {4001, "spellcrest::PlayerControls"}
};

const std::unordered_map<std::string, worker::ComponentId> schema_component_ids {
            {"improbable::Metadata", 53},
            {"improbable::Position", 54},
            {"godotcore::GodotPosition2D", 3001},
            {"spellcrest::PlayerControls", 4001}
};

class SchemaParser {
    private:
        static WorkerLogger logger;

    public:
        static Dictionary parseType(const improbable::Coordinates& data);
        static Dictionary parseComponent(const improbable::PositionData& data);

        static Dictionary parseComponent(const improbable::MetadataData& data);

        static Dictionary parseType(const godotcore::GodotChunk2D& data);
        static Dictionary parseType(const godotcore::GodotCoordinates2D& data);
        static Dictionary parseType(const godotcore::GodotVector2D& data);
        static Dictionary parseComponent(const godotcore::GodotPosition2DData& data);

        static Dictionary parseComponent(const spellcrest::PlayerControlsData& data);
};

#endif