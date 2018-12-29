
#include "editor_node.h"
#include "worker_logger.h"
#include "auto_instantiator.h"
#include "world_view.h"
#include "entity_view.h"
#include "component_view.h"
#include <godotcore/auto_instantiable.h>
#include <improbable/worker.h>
#include "spatial_util.h"
#include "scene/resources/packed_scene.h"

WorkerLogger AutoInstantiator::logger = WorkerLogger("auto_instantiator");

void AutoInstantiator::_on_entity_added(Node* added) {
    EntityView* entity_view = dynamic_cast<EntityView*>(added);
    if (entity_view == nullptr) {
        logger.error("Received an entity added that was not an entity view node");
        return;
    }
    if (!entity_view->has_component(godotcore::AutoInstantiable::ComponentId)) {
        // We only care about AutoInstantiable annotated entities here.
        return;
    }

    std::string scene_name = dynamic_cast<ComponentView<godotcore::AutoInstantiable>*>(entity_view
        ->getComponentNode(godotcore::AutoInstantiable::ComponentId))
        ->getData().scene_name();
    Ref<PackedScene> resource;
    bool found_in_cache = false;
    auto it = instantiable_scenes.find(scene_name);
    if (it != instantiable_scenes.end()) {
        resource = it->second;
        found_in_cache = true;
    } else {
        resource = ResourceLoader::load(toGodotString(scene_name));
        if (!resource.is_valid()) {
            logger.error("Could not load scene " + scene_name + " for entity " + std::to_string(entity_view->entity_id));
            return;
        }
    }

    Node* n = resource.ptr()->instance();
    if (n == nullptr) {
        logger.error("Failed to instance scene " + scene_name);
        return;
    }
    if (!(n->has_method("_setup_from_entity"))) {
        logger.warn(scene_name + " has no _setup_from_entity method despite being an AutoInstantiable, so will be ignored.");
        return;
    }

    // At this point it's safe to cache the loaded resource.
    if (!found_in_cache) {
        instantiable_scenes.insert({{scene_name, resource}});
        logger.info("Cached resource " + scene_name);
    }

    n->call("_setup_from_entity", entity_view);
    added_instances.insert({{entity_view->entity_id, n}});

    if (instantiator_target == nullptr) {
        instantiator_target = get_node(instantiator_path);
        if (instantiator_target == nullptr) {
            logger.warn("No target scene was set for the Auto Instantiator. Instancing entity as its own child");
            add_child(n);
        } else {
            instantiator_target->add_child(n);
        }
    } else {
        instantiator_target->add_child(n);
    }
}

void AutoInstantiator::_on_entity_removed(Node* removed) {
    EntityView* entity_view = dynamic_cast<EntityView*>(removed);
    if (entity_view == nullptr) {
        logger.error("Received an entity removed that was not an entity view node");
        return;
    }
    if (added_instances.find(entity_view->entity_id) == added_instances.end()) {
        return;
    }
    added_instances[entity_view->entity_id]->queue_delete();
    added_instances.erase(entity_view->entity_id);
}

void AutoInstantiator::set_instantiator_target(const NodePath &target) {
    instantiator_path = target;
    instantiator_target = get_node(instantiator_path);
}

NodePath AutoInstantiator::get_instantiator_target() const {
    return instantiator_path;
}

void AutoInstantiator::start(WorldView* world_view) {
    // Temporary and hacky but what are you going to do, stop me?
    if (world_view == nullptr) {
        logger.error("Unable to locate WorldView in scene, cannot initialise AutoInstantiator");
        return;
    }
    world_view->connect("entity_added", this, "_on_entity_added");
    world_view->connect("entity_removed", this, "_on_entity_removed");
    logger.info("Initialised AutoInstantiator");
}

AutoInstantiator::AutoInstantiator() {
}
