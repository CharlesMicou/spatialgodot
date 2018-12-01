#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <spellcrest/player_controls.h>
#include "component_registry.h"

const worker::ComponentRegistry& MergedComponentRegistry() {
    static const worker::Components<
        improbable::Position,
        improbable::Metadata,
        improbable::EntityAcl,
        improbable::Persistence,
        godotcore::GodotPosition2D,
        spellcrest::PlayerControls,
        spellcrest::ChatParticipant> components;
    return components;
}

const improbable::WorkerRequirementSet makeUniqueReqSet(std::string worker_id) {
    return improbable::WorkerRequirementSet{{{{"workerId:" + worker_id}}}};
}
