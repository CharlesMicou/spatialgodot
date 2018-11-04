#ifndef COMPONENT_VIEW_H
#define COMPONENT_VIEW_H

#include "editor_node.h"
#include <improbable/worker.h>

template <class T>
class ComponentView : public Node {
    GDCLASS(ComponentView, Node);
    worker::ComponentId componentId;
    bool authoritative;
    bool data; // todo(figure out how to represent schema)

protected:
    static void _bind_methods();

public:
    void authorityChange(const worker::Authority& authority);
    void updateComponent(const worker::ComponentUpdateOp<T>& update);
    void removeComponent();
    ComponentView();
};

#endif