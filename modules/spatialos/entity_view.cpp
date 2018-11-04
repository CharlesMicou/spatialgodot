#include "entity_view.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>

void EntityView::_bind_methods() {
    ADD_SIGNAL(MethodInfo("component_added", PropertyInfo(Variant::OBJECT, "entity_view", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
    ADD_SIGNAL(MethodInfo("component_removed", PropertyInfo(Variant::OBJECT, "entity_view", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
}

void EntityView::authorityChange(const worker::ComponentId, const worker::Authority& authority) {
    std::cout << "Received an authority change" << std::endl;
}

template <typename T>
void EntityView::addComponent(const worker::AddComponentOp<T>& add) {
    std::cout << "Received an add component" << std::endl;
}

template <typename T>
void EntityView::updateComponent(const worker::ComponentUpdateOp<T>& update) {
    std::cout << "Received an update component" << std::endl;
}

void EntityView::removeComponent(const worker::ComponentId) {
    std::cout << "Received a remove component" << std::endl;
}

EntityView::EntityView() {
}

template void EntityView::addComponent<improbable::Position>(const worker::AddComponentOp<improbable::Position>&);
template void EntityView::updateComponent<improbable::Position>(const worker::ComponentUpdateOp<improbable::Position>&);