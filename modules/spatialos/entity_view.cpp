#include "entity_view.h"
#include "component_view.h"
#include "schema_parser.h"
#include "spatial_util.h"
#include <string>
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <godotcore/auto_instantiable.h>
#include <godotcore/tile_map_chunk.h>
#include <spellcrest/moba_unit.h>
#include <spellcrest/player_controls.h>

WorkerLogger EntityView::logger = WorkerLogger("entity_view");

void EntityView::_bind_methods() {
    ADD_SIGNAL(MethodInfo("component_added", PropertyInfo(Variant::OBJECT, "component_view", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
    ADD_SIGNAL(MethodInfo("component_removed", PropertyInfo(Variant::OBJECT, "component_view", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
    ClassDB::bind_method(D_METHOD("get_entity_id"), &EntityView::get_entity_id);
    ClassDB::bind_method(D_METHOD("has_component_by_id", "component_id"), &EntityView::has_component);
    ClassDB::bind_method(D_METHOD("has_component", "component_name"), &EntityView::has_component_by_name);
    ClassDB::bind_method(D_METHOD("get_component_node_by_id", "component_id"), &EntityView::getComponentNode);
    ClassDB::bind_method(D_METHOD("get_component_node", "component_name"), &EntityView::getComponentNodeByName);
    ClassDB::bind_method(D_METHOD("get_all_component_values"), &EntityView::get_all_component_values);
}

void EntityView::authorityChange(const worker::ComponentId component_id, const worker::Authority& authority) {
    auto it = components.find(component_id);
    if (it != components.end()) {
        (it->second)->authorityChange(authority);
    }
}

template <typename T>
void EntityView::addComponent(const worker::AddComponentOp<T>& add) {
    ComponentView<T>* newComponent = memnew(ComponentView<T>);
    newComponent->init(entity_id, T::ComponentId, add.Data);
    // if this happens within a critical section no one will hear it.
    // instead, they'll receive a complete entity.
    // this is fine.
    components.insert({{T::ComponentId, newComponent}});

    // If the entity is already part of the scene, then connect the component
    // straight away.
    if (spos != NULL) {
        newComponent->setupConnection(spos);
    }
    add_child(newComponent);
    emit_signal("component_added", newComponent);
}

template <typename T>
void EntityView::updateComponent(const worker::ComponentUpdateOp<T>& update) {
    auto it = components.find(T::ComponentId);
    if (it != components.end()) {
        dynamic_cast<ComponentView<T>*>(it->second)->updateComponent(update);
    } else {
        logger.warn("Received a component update before it was added to the entity model");
    }
}

void EntityView::removeComponent(const worker::ComponentId component_id) {
    emit_signal("component_removed", components[component_id]);
    remove_child(components[component_id]);
    components.erase(component_id);
}

std::int64_t EntityView::get_entity_id() {
    return entity_id;
}

bool EntityView::has_component(const worker::ComponentId component_id) {
     auto it = components.find(component_id);
     return it != components.end();
}

bool EntityView::has_component_by_name(const String component_name) {
    std::string s = fromGodotString(component_name);
    auto it = schema_component_ids.find(s);
    if (it == schema_component_ids.end()) {
        logger.warn("The component " + s + " is not a known component in schema");
        return false;
    } else {
        return has_component(it->second);
    }
}

ComponentViewBase* EntityView::getComponentNode(const worker::ComponentId component_id) {
    auto it = components.find(component_id);
    if (it != components.end()) {
        return it->second;
    } else {
        long l = get_entity_id();
        logger.warn("The component " + std::to_string(component_id) + " is not present on entity " + std::to_string(l));
        return nullptr;
    }
}

ComponentViewBase* EntityView::getComponentNodeByName(const String component_name) {
    std::string s = fromGodotString(component_name);
    auto it = schema_component_ids.find(s);
    if (it == schema_component_ids.end()) {
        logger.warn("The component " + s + " is not a known component in schema");
        return nullptr;
    } else {
        return getComponentNode(it->second);
    }
}

Dictionary EntityView::get_all_component_values() const {
    Dictionary d;
    for (auto const& entry : components) {
        auto it = schema_component_names.find(entry.first);
        if (it != schema_component_names.end()) {
            d[toGodotString(it->second)] = entry.second->to_gd_dict();
        }
    }
    return d;
}

void EntityView::_notification(int p_what) {
    if (p_what == NOTIFICATION_ENTER_TREE) {
        Node* wv = get_parent();
        if (wv == NULL) {
            logger.error("Entity received enter tree but has no parent");
            return;
        }
        spos = wv->get_parent();
        if (spos == NULL) {
            logger.error("Entity entering scene could not find Spatialos node");
            return;
        }
        for (auto const& component : components) {
            component.second->setupConnection(spos);
        }
    }
}

EntityView::EntityView() {
}

// Force generation so that linking works
template void EntityView::addComponent<improbable::Position>(const worker::AddComponentOp<improbable::Position>&);
template void EntityView::updateComponent<improbable::Position>(const worker::ComponentUpdateOp<improbable::Position>&);

template void EntityView::addComponent<improbable::EntityAcl>(const worker::AddComponentOp<improbable::EntityAcl>&);
template void EntityView::updateComponent<improbable::EntityAcl>(const worker::ComponentUpdateOp<improbable::EntityAcl>&);

template void EntityView::addComponent<improbable::Metadata>(const worker::AddComponentOp<improbable::Metadata>&);
template void EntityView::updateComponent<improbable::Metadata>(const worker::ComponentUpdateOp<improbable::Metadata>&);

template void EntityView::addComponent<godotcore::GodotPosition2D>(const worker::AddComponentOp<godotcore::GodotPosition2D>&);
template void EntityView::updateComponent<godotcore::GodotPosition2D>(const worker::ComponentUpdateOp<godotcore::GodotPosition2D>&);

template void EntityView::addComponent<godotcore::AutoInstantiable>(const worker::AddComponentOp<godotcore::AutoInstantiable>&);
template void EntityView::updateComponent<godotcore::AutoInstantiable>(const worker::ComponentUpdateOp<godotcore::AutoInstantiable>&);

template void EntityView::addComponent<godotcore::TileMapChunk>(const worker::AddComponentOp<godotcore::TileMapChunk>&);
template void EntityView::updateComponent<godotcore::TileMapChunk>(const worker::ComponentUpdateOp<godotcore::TileMapChunk>&);

template void EntityView::addComponent<spellcrest::PlayerControls>(const worker::AddComponentOp<spellcrest::PlayerControls>&);
template void EntityView::updateComponent<spellcrest::PlayerControls>(const worker::ComponentUpdateOp<spellcrest::PlayerControls>&);

template void EntityView::addComponent<spellcrest::ChatParticipant>(const worker::AddComponentOp<spellcrest::ChatParticipant>&);
template void EntityView::updateComponent<spellcrest::ChatParticipant>(const worker::ComponentUpdateOp<spellcrest::ChatParticipant>&);

template void EntityView::addComponent<spellcrest::MobaUnit>(const worker::AddComponentOp<spellcrest::MobaUnit>&);
template void EntityView::updateComponent<spellcrest::MobaUnit>(const worker::ComponentUpdateOp<spellcrest::MobaUnit>&);
