#include "world_view.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>

void WorldView::_bind_methods() {
    ADD_SIGNAL(MethodInfo("entity_added", PropertyInfo(Variant::OBJECT, "entity_view", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
    ADD_SIGNAL(MethodInfo("entity_removed", PropertyInfo(Variant::OBJECT, "entity_view", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
}

void WorldView::addEntity(const worker::AddEntityOp& add) {
    EntityView* newView = memnew(EntityView);
    entities.insert({{add.EntityId, newView}});

    // don't add the entity in the middle of a critical section
    if (inCriticalSection) {
        std::cout << "Received an add entity, but inside a critical section." << std::endl;
        pendingSceneEntities.push(add.EntityId);
    } else {
        addEntityToScene(add.EntityId);
    }
}

void WorldView::removeEntity(const worker::RemoveEntityOp& remove) {
    std::cout << "Emitting a remove entity signal" << std::endl;
    emit_signal("entity_removed", entities[remove.EntityId]);
    remove_child(entities[remove.EntityId]);
    entities.erase(remove.EntityId);
}

void WorldView::authorityChange(const worker::EntityId entity_id, worker::ComponentId component_id, const worker::Authority& authority) {
    entities[entity_id]->authorityChange(component_id, authority);
}

template <typename T>
void WorldView::addComponent(const worker::AddComponentOp<T>& add) {
    entities[add.EntityId]->addComponent(add);
}

template <typename T>
void WorldView::updateComponent(const worker::ComponentUpdateOp<T>& update) {
    entities[update.EntityId]->updateComponent(update);
}

void WorldView::removeComponent(const worker::EntityId entity_id, worker::ComponentId component_id) {
    entities[entity_id]->removeComponent(component_id);
}

void WorldView::handleCriticalSection(const worker::CriticalSectionOp& section) {
    inCriticalSection = section.InCriticalSection;
    if (!inCriticalSection) {
        while (!pendingSceneEntities.empty()) {
            addEntityToScene(pendingSceneEntities.front());
            pendingSceneEntities.pop();
        }
    }
}

void WorldView::addEntityToScene(worker::EntityId entity_id) {
    add_child(entities[entity_id]);
    std::cout << "Emitting an add entity signal" << std::endl;
    emit_signal("entity_added", entities[entity_id]);
}

WorldView::WorldView() {
    inCriticalSection = false;
}

// Force generation so that linking works
template void WorldView::addComponent<improbable::Position>(const worker::AddComponentOp<improbable::Position>&);
template void WorldView::updateComponent<improbable::Position>(const worker::ComponentUpdateOp<improbable::Position>&);
