#ifndef COMPONENT_VIEW_H
#define COMPONENT_VIEW_H

#include "editor_node.h"
#include "worker_logger.h"
#include <improbable/worker.h>


class ComponentViewBase : public Node {
    public:
        virtual void authorityChange(const worker::Authority& authority) = 0;
};

template <typename T>
class ComponentView : public ComponentViewBase {
    GDCLASS(ComponentView, Node);
    static WorkerLogger logger;

    bool authoritative;
    typename T::Data data;
    worker::ComponentId componentId;

protected:
    static void _bind_methods();

public:
    void authorityChange(const worker::Authority& authority) override;
    void updateComponent(const worker::ComponentUpdateOp<T>& update);
    void removeComponent();

    void init(const worker::ComponentId component_id, const typename T::Data& state);
    const bool hasAuthority();
    const typename T::Data& getData();

    ComponentView();
};

#endif