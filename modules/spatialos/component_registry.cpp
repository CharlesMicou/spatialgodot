#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <godotcore/auto_instantiable.h>
#include <spellcrest/player_controls.h>
#include "component_registry.h"

worker::Map<worker::ComponentId, improbable::WorkerRequirementSet> make_component_acl(const worker::Entity& entity, const std::string player_worker_id) {
    worker::Map<worker::ComponentId, improbable::WorkerRequirementSet> builder;
    worker::List<worker::ComponentId> component_ids = entity.GetComponentIds();
    auto it = component_ids.begin();
    while (it != component_ids.end()) {
        auto should_simulate = simulatedComponents.find(*it);
        if (should_simulate != simulatedComponents.end()) {
            builder.insert({{*it, serverReqSet}});
        }
        if (player_worker_id != "") {
            auto player_should_be_authoritative = playerAuthoritativeComponents.find(*it);
            if (player_should_be_authoritative != playerAuthoritativeComponents.end()) {
                builder.insert({{*it, makeUniqueReqSet(player_worker_id)}});
            }
        }
        it++;
    }
    return builder;
}

const worker::ComponentRegistry& MergedComponentRegistry() {
    static const worker::Components<
        improbable::Position,
        improbable::Metadata,
        improbable::EntityAcl,
        improbable::Persistence,
        godotcore::GodotPosition2D,
        godotcore::AutoInstantiable,
        spellcrest::PlayerControls,
        spellcrest::ChatParticipant> components;
    return components;
}

const improbable::WorkerRequirementSet makeUniqueReqSet(std::string worker_id) {
    return improbable::WorkerRequirementSet{{{{"workerId:" + worker_id}}}};
}
