#include "component_view.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include "spatial_util.h"
#include <godotcore/godot_position2d.h>
#include <spellcrest/player_controls.h>
#include "spatialos.h"
#include "schema_parser.h"

const worker::ComponentId kPositionId = 54;
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

// Force generation so that linking works
template class ComponentView<improbable::Position>;
template class ComponentView<improbable::Metadata>;
template class ComponentView<godotcore::GodotPosition2D>;
template class ComponentView<spellcrest::PlayerControls>;
template class ComponentView<spellcrest::ChatParticipant>;
