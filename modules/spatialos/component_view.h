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
        virtual void setupConnection(Node* spos) = 0;
};

template <typename T>
class ComponentView : public ComponentViewBase {
    static WorkerLogger logger;

    bool initialized;

    // Absolutely horrific hack to get a reference to the Spatialos node
    Node* connection;
    bool authoritative;
    typename T::Data data;
    worker::ComponentId componentId;
    worker::EntityId entityId;

public:
    // Processing signals sent to the worker
    void authorityChange(const worker::Authority& authority) override;
    void updateComponent(const worker::ComponentUpdateOp<T>& update);
    void removeComponent();

    // Sending updates back upstream
    bool tryUpdate(const typename T::Update& update);

    // Initialisation
    void setupConnection(Node* spos);
    void init(const worker::EntityId entity_id, const worker::ComponentId component_id, const typename T::Data& state);

    const bool hasAuthority();
    const typename T::Data& getData();

    ComponentView();
};

#endif