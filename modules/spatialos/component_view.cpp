#include "component_view.h"
#include "entity_view.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include "spatial_util.h"
#include <godotcore/godot_position2d.h>
#include <godotcore/auto_instantiable.h>
#include <godotcore/tile_map_chunk.h>
#include <spellcrest/player_controls.h>
#include <spellcrest/moba_unit.h>
#include "spatialos.h"
#include "schema_parser.h"
#include "component_registry.h"

template <typename T>
WorkerLogger ComponentView<T>::logger = WorkerLogger("component_view");

template <typename T>
void ComponentView<T>::authorityChange(const worker::Authority& authority) {
    bool prev = authoritative;
    authoritative = (authority == worker::Authority::kAuthoritative);
    if (authoritative != prev) {
        emit_signal("authority_changed", authoritative);
    }
}

template <typename T>
void ComponentView<T>::setupConnection(Node* spos) {
    connection = spos;
}

template <typename T>
void ComponentView<T>::updateComponent(const worker::ComponentUpdateOp<T>& update) {
    if (!initialized) {
        logger.warn("Tried to apply an update for component " + std::to_string(componentId) + " on entity " + std::to_string(entityId)
            + " before it was initialized.");
    }
    update.Update.ApplyTo(data);
    for (auto it : SchemaParser::extractEvents(update.Update)) {
        emit_signal("component_event", it);
    }
    emit_signal("component_updated");
}

template <typename T>
void ComponentView<T>::init(const worker::EntityId entity_id, const worker::ComponentId component_id, const typename T::Data& state) {
    if (initialized) {
        logger.warn("Component " + std::to_string(componentId) + " on entity " + std::to_string(entityId)
            + " was already initialized, but init was called.");
    }
    componentId = component_id;
    data = state;
    entityId = entity_id;
    initialized = true;
}

template <typename T>
bool ComponentView<T>::tryUpdate(const typename T::Update& update) {
    if (!initialized) {
        logger.warn("Tried to send an update for component " + std::to_string(componentId) + " on entity " + std::to_string(entityId)
            + " before it was initialized.");
        return false;
    }
    if (connection == nullptr) {
            // not ready to send yet
            return false;
    }
    if (!authoritative) {
        return false;
    }
    Spatialos* s = dynamic_cast<Spatialos*>(connection);
    if (s == nullptr) {
        logger.error("Component " + std::to_string(componentId) + " on entity " + std::to_string(entityId)
            + " was unable to find the Spatialos node.");
        return false;
    }
    s->sendComponentUpdate<T>(entityId, update);
    return true;
}

template <typename T>
bool ComponentView<T>::try_update(const Dictionary d) {
    typename T::Update update;
    SchemaParser::serializeComponentUpdate(update, d);
    return tryUpdate(update);
}

template <typename T>
const typename T::Data& ComponentView<T>::getData() {
    return data;
}

template <typename T>
Dictionary ComponentView<T>::to_gd_dict() const {
    const typename T::Data& d = data;
    return SchemaParser::parseComponent(d);
}

template <typename T>
ComponentView<T>::ComponentView() {
    authoritative = false;
    initialized = false;
}

template <typename T>
bool ComponentView<T>::hasAuthority() {
    return authoritative;
}

template <typename T>
bool ComponentView<T>::canHaveAuthority() {
    Spatialos* s = dynamic_cast<Spatialos*>(connection);
    if (s == nullptr) {
        logger.error("Tried to determine if component " + std::to_string(componentId) + " on entity " + std::to_string(entityId)
            + " could be authoritative, but it did not have the Spos connection initialised. Did you call it before it was fully initialised?");
        return false;
    }

    bool isSimulated = simulatedComponents.find(componentId) != simulatedComponents.end();
    bool isPlayer = playerAuthoritativeComponents.find(componentId) != playerAuthoritativeComponents.end();

    // The simplest case: no one is ever authoritative over this component
    if (!isSimulated && !isPlayer) {
        return false;
    }

    // If this is a server worker, then we can get authority over the component if it is a server-simulated component.
    if (s->isServerWorker()) {
        return isSimulated;
    }

    // From here on out, we know that this is a player worker rather than a server worker.
    // If this is a simulated component, player workers will not be authoritative over it.
    if (isSimulated) {
        return false;
    }

    // The complex case: this is a player-simulated component, and we need to figure out if we are the right player for it.
    EntityView* entity = dynamic_cast<EntityView*>(get_parent());
    if (entity == NULL) {
        logger.error("Tried to determine if component " + std::to_string(componentId) + " on entity " + std::to_string(entityId)
            + " could be authoritative, but it did not have an entity view parent. Did you call it before it was fully initialised?");
        return false;
    }
    ComponentView<improbable::EntityAcl>* entityAcl = dynamic_cast<ComponentView<improbable::EntityAcl>*>(entity->getComponentNode(improbable::EntityAcl::ComponentId));
    if (entityAcl == NULL) {
        // Adding a warning here as this should *probably* not be happening, and if it does I'd like to know about it.
        logger.warn("Tried to determine if component " + std::to_string(componentId) + " on entity " + std::to_string(entityId)
            + " could be authoritative, but could not find an ACL component view for that entity.");
        return false;
    }
    const worker::Map<worker::ComponentId, improbable::WorkerRequirementSet> write_acl = entityAcl->getData().component_write_acl();
    auto it = write_acl.find(componentId);
    if (it == write_acl.end()) {
        // Not present in the write acl means never authoritative.
        return false;
    }
    if (it->second == makeUniqueReqSet(s->getWorkerId())) {
        // The ACL defines a value for this component id, and that component id corresponds to the worker requirement set uniquely identifying this worker
        return true;
    }
    // Even though we found an ACL definition, it corresponds to a different unique worker, so this worker will never get authority
    return false;
}

// Force generation so that linking works
template class ComponentView<improbable::Position>;
template class ComponentView<improbable::EntityAcl>;
template class ComponentView<improbable::Metadata>;
template class ComponentView<godotcore::GodotPosition2D>;
template class ComponentView<godotcore::AutoInstantiable>;
template class ComponentView<godotcore::TileMapChunk>;
template class ComponentView<spellcrest::PlayerControls>;
template class ComponentView<spellcrest::ChatParticipant>;
template class ComponentView<spellcrest::MobaUnit>;

