#ifndef SPOS_AUTO_INSTANTIATOR_H
#define SPOS_AUTO_INSTANTIATOR_H

#include "editor_node.h"
#include "worker_logger.h"
#include "world_view.h"
#include <improbable/worker.h>

class AutoInstantiator : public Node {
    GDCLASS(AutoInstantiator, Node);
    worker::Map<worker::EntityId, Node*> added_instantances;
    
    // TODO: make this a cache with an eviction policy, rather than
    // an obvious memory leak.
    worker::Map<std::string, RES> instantiable_scenes;

    // TODO: support configuring multiple kinds of target node
    NodePath instantiator_path;
    Node* instantiator_target;

    protected:
        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("_on_entity_added", "entity_view"), &AutoInstantiator::_on_entity_added);
            ClassDB::bind_method(D_METHOD("_on_entity_removed", "entity_view"), &AutoInstantiator::_on_entity_removed);

            ClassDB::bind_method(D_METHOD("set_instantiator_target", "target"), &AutoInstantiator::set_instantiator_target);
	        ClassDB::bind_method(D_METHOD("get_instantiator_target"), &AutoInstantiator::get_instantiator_target);
            ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "instantiator_target"), "set_instantiator_target", "get_instantiator_target");
        }

    public:
        void _on_entity_added(Node* added);
        void _on_entity_removed(Node* removed);
        void start(WorldView* world_view);

        void set_instantiator_target(const NodePath &target);
	    NodePath get_instantiator_target() const;

        static WorkerLogger logger;

        // TODO: add a preload call that loads folders of instantiables
        AutoInstantiator();
};
#endif
