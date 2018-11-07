#ifndef COMPONENT_VIEW_H
#define COMPONENT_VIEW_H

#include "editor_node.h"
#include <improbable/worker.h>

class ComponentView : public Node {
    GDCLASS(ComponentView, Node);
    bool authoritative;
    bool data; // todo(figure out how to represent schema)
    Vector2 syncedPos;

protected:
    static void _bind_methods();

public:
    void authorityChange(const worker::Authority& authority);
    template <class T>
    void updateComponent(const worker::ComponentUpdateOp<T>& update);
    void removeComponent();

    // This is temporary while I figure out spawning and stuff.
    Vector2 getPosition();

    // todo: make this private because this is crazy dangerous
    worker::ComponentId componentId;
    ComponentView();
};

#endif