#ifndef COMPONENT_VIEW_H
#define COMPONENT_VIEW_H

#include "editor_node.h"
#include <improbable/worker.h>


class ComponentViewBase : public Node {
    public:
        virtual void authorityChange(const worker::Authority& authority) = 0;
};

template <typename T>
class ComponentView : public ComponentViewBase {
    GDCLASS(ComponentView, Node);
    bool authoritative;
    typename T::Data data;
    worker::ComponentId componentId;
    Vector2 syncedPos;

protected:
    static void _bind_methods();

public:
    void authorityChange(const worker::Authority& authority) override;
    void updateComponent(const worker::ComponentUpdateOp<T>& update);
    void removeComponent();

    // This is temporary while I figure out spawning and stuff.
    Vector2 getPosition();
    void init(const worker::ComponentId component_id, const typename T::Data& state);
    const bool hasAuthority();

    ComponentView();
};

#endif