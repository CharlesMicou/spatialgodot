#ifndef ENTITY_VIEW_H
#define ENTITY_VIEW_H

#include "editor_node.h"
#include "component_view.h"
#include <improbable/worker.h>

class EntityView : public Node {
    GDCLASS(EntityView, Node);
    worker::EntityId entity_id;
    worker::Map<worker::ComponentId, ComponentViewBase*> components;

protected:
    static void _bind_methods();

public:
    void authorityChange(const worker::ComponentId component_id, const worker::Authority& authority);
    template <typename T>
    void addComponent(const worker::AddComponentOp<T>& add);
    template <typename T>
    void updateComponent(const worker::ComponentUpdateOp<T>& update);
    void removeComponent(const worker::ComponentId component_id);
    // todo: expose a get all component nodes method
    
    EntityView();
};

#endif