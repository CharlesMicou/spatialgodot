#ifndef WORLD_VIEW_H
#define WORLD_VIEW_H

#include "editor_node.h"
#include <improbable/worker.h>

class WorldView : public Node2D {
    GDCLASS(WorldView, Node2D);

protected:
    static void _bind_methods();

public:
    void addEntity(const worker::AddEntityOp& add);
    void removeEntity(const worker::RemoveEntityOp& remove);
    void authorityChange(const worker::AuthorityChangeOp authority);
    template <typename T>
    void addComponent(const worker::AddComponentOp<T> add);
    template <typename T>
    void updateComponent(const worker::ComponentUpdateOp<T> update);
    void removeComponent(const worker::RemoveComponentOp remove);
    
    WorldView();
    ~WorldView();
};

#endif