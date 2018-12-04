#ifndef SPOS_COMPONENT_REGISTRY_H
#define SPOS_COMPONENT_REGISTRY_H

#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <godotcore/auto_instantiable.h>
#include <godotcore/godot_position2d.h>
#include <unordered_set>

const worker::ComponentRegistry& MergedComponentRegistry();

const improbable::WorkerRequirementSet serverReqSet{{{{"gserver"}}}};
const improbable::WorkerRequirementSet clientAndServerReqSet{{{{"gserver"}}, {{"gclient"}}}};
const improbable::WorkerRequirementSet makeUniqueReqSet(std::string worker_id);

const std::unordered_set<worker::ComponentId> simulatedComponents{
    improbable::Position::ComponentId,
    godotcore::GodotPosition2D::ComponentId
};

#endif