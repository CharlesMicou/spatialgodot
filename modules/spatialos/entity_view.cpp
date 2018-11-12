#include "entity_view.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>
#include <stdio.h>

void EntityView::_bind_methods() {
    ADD_SIGNAL(MethodInfo("component_added", PropertyInfo(Variant::OBJECT, "component_view", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
    ADD_SIGNAL(MethodInfo("component_removed", PropertyInfo(Variant::OBJECT, "component_view", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
    ClassDB::bind_method(D_METHOD("get_entity_id"), &EntityView::get_entity_id);
}

void EntityView::authorityChange(const worker::ComponentId component_id, const worker::Authority& authority) {
    std::cout << "Received an authority change" << std::endl;
    auto it = components.find(component_id);
    if (it != components.end()) {
        std::cout << "Forwarding authority change" << std::endl;
        (it->second)->authorityChange(authority);
    }
}

template <typename T>
void EntityView::addComponent(const worker::AddComponentOp<T>& add) {
    std::cout << "Received an add component" << std::endl;
    ComponentView<T>* newComponent = memnew(ComponentView<T>);
    newComponent->init(T::ComponentId, add.Data);
    // if this happens within a critical section no one will hear it.
    // instead, they'll receive a complete entity.
    // this is fine.
    components.insert({{T::ComponentId, newComponent}});
    add_child(newComponent);
    emit_signal("component_added", newComponent);
}

template <typename T>
void EntityView::updateComponent(const worker::ComponentUpdateOp<T>& update) {
    /*auto it = components.find(T::ComponentId);
    if (it != components.end()) {
        std::cout << "Handling a component update" << std::endl;
        dynamic_cast<ComponentView<T>*>(it->second)->updateComponent(update);
    } else {
        std::cout << "Received a component update before it was added to the entity model" << std::endl;
    }*/
}

void EntityView::removeComponent(const worker::ComponentId component_id) {
    std::cout << "Received a remove component" << std::endl;
    emit_signal("component_removed", components[component_id]);
    remove_child(components[component_id]);
    components.erase(component_id);
}

std::int64_t EntityView::get_entity_id() {
    return entity_id;
}

EntityView::EntityView() {
}

// Force generation so that linking works
template void EntityView::addComponent<improbable::Position>(const worker::AddComponentOp<improbable::Position>&);
template void EntityView::updateComponent<improbable::Position>(const worker::ComponentUpdateOp<improbable::Position>&);
//template void EntityView::addComponent<improbable::Metadata>(const worker::AddComponentOp<improbable::Metadata>&);
//template void EntityView::updateComponent<improbable::Metadata>(const worker::ComponentUpdateOp<improbable::Metadata>&);