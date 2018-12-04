
#include "editor_node.h"
#include "worker_logger.h"
#include "auto_instantiable.h"
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
    String scene_name = toGodotString(dynamic_cast<ComponentView<godotcore::AutoInstantiable>*>(entity_view
        ->getComponentNode(godotcore::AutoInstantiable::ComponentId))
        ->getData().scene_name());
    Ref<PackedScene> resource = ResourceLoader::load(scene_name);
    if (!resource.is_valid()) {
        logger.error("Could not load scene " + fromGodotString(scene_name));
        return;
    }
    Node* n = resource.ptr()->instance();
    if (n == nullptr) {
        logger.error("Failed to instance scene " + fromGodotString(scene_name));
        return;
    }
    AutoInstantiableScene* s = dynamic_cast<AutoInstantiableScene*>(n);
    if (s == nullptr) {
        logger.error(fromGodotString(scene_name) + " is not an AutoInstantiableScene!");
        return;
    }
    // Invoke magic gdscript virtual method
    logger.info("Trying to invoke setup call on " + fromGodotString(scene_name));
    s->call("_setup_from_entity", entity_view);
    added_instantances.insert({{entity_view->entity_id, s}});
    logger.info(fromGodotString(scene_name) + " loaded for entity " + std::to_string(entity_view->get_entity_id()));
    add_child(s);
}

void AutoInstantiator::_on_entity_removed(Node* removed) {
    EntityView* entity_view = dynamic_cast<EntityView*>(removed);
    if (entity_view == nullptr) {
        logger.error("Received an entity removed that was not an entity view node");
        return;
    }
    remove_child(added_instantances[entity_view->entity_id]);
    added_instantances.erase(entity_view->entity_id);
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
