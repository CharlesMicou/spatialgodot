#ifndef SPOS_AUTO_INSTANTIABLE_H
#define SPOS_AUTO_INSTANTIABLE_H

#include "editor_node.h"
#include "worker_logger.h"
#include "world_view.h"
#include <improbable/worker.h>

class AutoInstantiableScene : public Node {
    GDCLASS(AutoInstantiableScene, Node);

    protected:
        static void _bind_methods() {
            BIND_VMETHOD(MethodInfo("_setup_from_entity", PropertyInfo(Variant::OBJECT, "entity_view")));
        }
};

class AutoInstantiator : public Node {
    GDCLASS(AutoInstantiator, Node);
    worker::Map<worker::EntityId, AutoInstantiableScene*> added_instantances;
    // Todo: cache loaded resources
    worker::Map<std::string, RES> instantiable_scenes;

    protected:
        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("_on_entity_added", "entity_view"), &AutoInstantiator::_on_entity_added);
            ClassDB::bind_method(D_METHOD("_on_entity_removed", "entity_view"), &AutoInstantiator::_on_entity_removed);
        }

    public:
        void _on_entity_added(Node* added);
        void _on_entity_removed(Node* removed);
        void start(WorldView* world_view);

        static WorkerLogger logger;

        // Todo: add a preload call that loads folders of instantiables
        AutoInstantiator();
};
#endif
