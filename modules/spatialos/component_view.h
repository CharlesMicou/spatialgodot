#ifndef COMPONENT_VIEW_H
#define COMPONENT_VIEW_H

#include "editor_node.h"
#include "worker_logger.h"
#include <improbable/worker.h>

/**
 * This base class serves two purposes:
 *   - the EntityView can hold collections of what are effectively type-erased component views
 *   - Godot's class db doesn't explode under trying to register template classes
 */
class ComponentViewBase : public Node {
    GDCLASS(ComponentViewBase, Node);

    protected:
        static void _bind_methods() {
            ADD_SIGNAL(MethodInfo("component_updated", PropertyInfo(Variant::OBJECT, "component_update", PROPERTY_HINT_NONE, "Reference")));
            ADD_SIGNAL(MethodInfo("authority_changed", PropertyInfo(Variant::BOOL, "authority")));
        }

    public:
        virtual void authorityChange(const worker::Authority& authority) = 0;
};

template <typename T>
class ComponentView : public ComponentViewBase {
    static WorkerLogger logger;

    bool authoritative;
    typename T::Data data;
    worker::ComponentId componentId;

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