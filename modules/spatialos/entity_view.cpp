#include "entity_view.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>
#include <stdio.h>

void EntityView::_bind_methods() {
    ADD_SIGNAL(MethodInfo("component_added", PropertyInfo(Variant::OBJECT, "component_view", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
    ADD_SIGNAL(MethodInfo("component_removed", PropertyInfo(Variant::OBJECT, "component_view", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
}

void EntityView::authorityChange(const worker::ComponentId component_id, const worker::Authority& authority) {
    std::cout << "Received an authority change" << std::endl;
}

template <typename T>
void EntityView::addComponent(const worker::AddComponentOp<T>& add) {
    std::cout << "Received an add component" << std::endl;
    ComponentView* newComponent = memnew(ComponentView);
    newComponent->componentId = T::ComponentId;
    // if this happens within a critical section no one will hear it.
    // instead, they'll receive a complete entity.
    // this is fine.
    components.insert({{T::ComponentId, newComponent}});
    add_child(newComponent);
    emit_signal("component_added", newComponent);
}

template <typename T>
void EntityView::updateComponent(const worker::ComponentUpdateOp<T>& update) {
    printf("Handling component update for component id %d\n", T::ComponentId);
    components[T::ComponentId]->updateComponent(update);
}

void EntityView::removeComponent(const worker::ComponentId component_id) {
    std::cout << "Received a remove component" << std::endl;
    emit_signal("component_removed", components[component_id]);
    remove_child(components[component_id]);
    components.erase(component_id);
}

EntityView::EntityView() {
}

// Force generation so that linking works
template void EntityView::addComponent<improbable::Position>(const worker::AddComponentOp<improbable::Position>&);
template void EntityView::updateComponent<improbable::Position>(const worker::ComponentUpdateOp<improbable::Position>&);