#ifndef SPOS_COMPONENT_REGISTRY_H
#define SPOS_COMPONENT_REGISTRY_H

#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <godotcore/auto_instantiable.h>
#include <godotcore/godot_position2d.h>
#include <godotcore/tile_map_chunk.h>
#include <spellcrest/player_controls.h>
#include <spellcrest/moba_unit.h>
#include <unordered_set>

const worker::ComponentRegistry& MergedComponentRegistry();

const improbable::WorkerRequirementSet serverReqSet{{{{"gserver"}}}};
const improbable::WorkerRequirementSet clientAndServerReqSet{{{{"gserver"}}, {{"gclient"}}}};
const improbable::WorkerRequirementSet makeUniqueReqSet(std::string worker_id);

worker::Map<worker::ComponentId, improbable::WorkerRequirementSet> make_component_acl(const worker::Entity& entity, const std::string player_worker_id);

const std::unordered_set<worker::ComponentId> simulatedComponents {
    improbable::Position::ComponentId,
    godotcore::GodotPosition2D::ComponentId,
    godotcore::TileMapChunk::ComponentId,
    spellcrest::MobaUnit::ComponentId
};

const std::unordered_set<worker::ComponentId> playerAuthoritativeComponents {
    spellcrest::PlayerControls::ComponentId,
    spellcrest::ChatParticipant::ComponentId
};

#endif