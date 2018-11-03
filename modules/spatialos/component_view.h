#ifndef ENTITY_VIEW_H
#define ENTITY_VIEW_H

#include "editor_node.h"
#include <improbable/worker.h>

class EntityView : public Node2D {
    GDCLASS(EntityView, Node2D);

protected:
    static void _bind_methods();

public:
    void authorityChange(const worker::AuthorityChangeOp& authority);
    template <typename T>
    void addComponent(const worker::AddComponentOp<T>& add);
    template <typename T>
    void updateComponent(const worker::ComponentUpdateOp<T>& update);
    void removeComponent(const worker::RemoveComponentOp& remove);
    EntityView();
};

#endif