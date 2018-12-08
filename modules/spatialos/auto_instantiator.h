#ifndef SPOS_AUTO_INSTANTIATOR_H
#define SPOS_AUTO_INSTANTIATOR_H

#include "editor_node.h"
#include "worker_logger.h"
#include "world_view.h"
#include <improbable/worker.h>

class AutoInstantiator : public Node {
    GDCLASS(AutoInstantiator, Node);
    worker::Map<worker::EntityId, Node*> added_instantances;
    
    // Todo: make this a cache with an eviction policy, rather than
    // an obvious memory leak.
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
