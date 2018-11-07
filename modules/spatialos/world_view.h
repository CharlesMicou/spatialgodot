#ifndef WORLD_VIEW_H
#define WORLD_VIEW_H

#include "editor_node.h"
#include "entity_view.h"
#include <improbable/worker.h>
#include <queue>

class WorldView : public Node2D {
    GDCLASS(WorldView, Node2D);
    bool inCriticalSection;
    worker::Map<worker::EntityId, EntityView*> entities;
    std::queue<worker::EntityId> pendingSceneEntities;

    void addEntityToScene(worker::EntityId entity_id);

protected:
    static void _bind_methods();

public:
    void addEntity(const worker::AddEntityOp& add);
    void removeEntity(const worker::RemoveEntityOp& remove);
    void authorityChange(const worker::EntityId entity_id, worker::ComponentId component_id, const worker::Authority& authority);
    template <typename T>
    void addComponent(const worker::AddComponentOp<T>& add);
    template <typename T>
    void updateComponent(const worker::ComponentUpdateOp<T>& update);
    void removeComponent(const worker::EntityId entity_id, worker::ComponentId component_id);
    void handleCriticalSection(const worker::CriticalSectionOp& section);
    
    WorldView();
};

#endif