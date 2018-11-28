#ifndef ENTITY_VIEW_H
#define ENTITY_VIEW_H

#include "editor_node.h"
#include "component_view.h"
#include <improbable/worker.h>

class EntityView : public Node {
    GDCLASS(EntityView, Node);
    static WorkerLogger logger;
    worker::Map<worker::ComponentId, ComponentViewBase*> components;
    Node* spos;

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    void authorityChange(const worker::ComponentId component_id, const worker::Authority& authority);
    template <typename T>
    void addComponent(const worker::AddComponentOp<T>& add);
    template <typename T>
    void updateComponent(const worker::ComponentUpdateOp<T>& update);
    void removeComponent(const worker::ComponentId component_id);
    std::int64_t get_entity_id();
    worker::EntityId entity_id;
    ComponentViewBase* getComponentNode(const worker::ComponentId component_id);
    
    EntityView();
};

#endif