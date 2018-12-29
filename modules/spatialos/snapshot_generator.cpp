#include "snapshot_generator.h"
#include <godotcore/godot_position2d.h>
#include <improbable/standard_library.h>
#include <godotcore/auto_instantiable.h>
#include <godotcore/tile_map_chunk.h>
#include "improbable/worker.h"
#include "spatial_util.h"
#include "component_registry.h"
#include "schema_parser.h"
#include "streaming_tile_map.h"

WorkerLogger SnapshotGenerator::logger = WorkerLogger("snapshot_generator");

void SnapshotGenerator::new_snapshot(String path) {
    snapshot_in_progress = true;
    std::string p = fromGodotString(path);
    snapshot_ostream = new worker::SnapshotOutputStream{MergedComponentRegistry(), p};
    logger.info("Writing snapshot to " + p);
    current_entity_id = 1;
}

void SnapshotGenerator::add_auto_instantiable_entity(Node* node) {
    if (snapshot_ostream == nullptr || !snapshot_in_progress) {
        logger.error("Tried to add an entity to the snapshot without calling new_snapshot first");
        return;
    }
    Node2D* n = dynamic_cast<Node2D*>(node);
    if (n == nullptr) {
        std::string offending_node = fromGodotString(node->get_name());
        logger.warn("The node " + offending_node + " was not a Node2D, therefore has no position. Not adding it.");
        return;
    }
    if (!n->has_method("_to_snapshot_entity")) {
        std::string offending_node = fromGodotString(node->get_name());
        logger.warn("The node " + offending_node + " has no method _to_snapshot_entity. Not adding it.");
        return;
    }
    if (!n->has_method("_setup_from_entity")) {
        std::string offending_node = fromGodotString(node->get_name());
        logger.warn("The node " + offending_node + " has no method _setup_from_entity so cannot be auto-instantiated, but add_auto_instantiable_entity was called.");
        return;
    }
    String resource_path = node->get_filename();
    String default_metadata = resource_path.get_file();

    worker::Entity entity;
    current_entity_id++;
    godotcore::GodotCoordinates2D position = toGlobalGodotPosition(n->get_global_position(), 0, 0);
    entity.Add<godotcore::GodotPosition2D>({position, {}});
    entity.Add<improbable::Position>(fromGodotPosition(position));
    entity.Add<improbable::Persistence>({});
    entity.Add<improbable::Metadata>({fromGodotString(default_metadata)});
    entity.Add<godotcore::AutoInstantiable>({fromGodotString(resource_path)});
    SchemaParser::applyComponentsToEntity(entity, n->call("_to_snapshot_entity"));
    entity.Add<improbable::EntityAcl>({clientAndServerReqSet, make_component_acl(entity, "")});

    snapshot_ostream->WriteEntity(current_entity_id, entity);
}

void SnapshotGenerator::add_streaming_tile_map_entities(Node* node) {
    if (snapshot_ostream == nullptr || !snapshot_in_progress) {
        logger.error("Tried to add an entity to the snapshot without calling new_snapshot first");
        return;
    }
    StreamingTileMap* tile_map = dynamic_cast<StreamingTileMap*>(node);
    if (tile_map == nullptr) {
        std::string offending_node = fromGodotString(node->get_name());
        logger.warn("The node " + offending_node + " was not a StreamingTileMap. Not adding it.");
        return;
    }

    std::list<worker::Entity> tile_map_chunks = tile_map->to_snapshot_entities();
    for (auto entity : tile_map_chunks) {
        current_entity_id++;
        godotcore::GodotCoordinates2D position = entity.Get<godotcore::GodotPosition2D>()->coordinates();
        entity.Add<improbable::Position>(fromGodotPosition(position));
        entity.Add<improbable::Persistence>({});
        entity.Add<improbable::Metadata>({"MapChunk"});
        entity.Add<improbable::EntityAcl>({clientAndServerReqSet, make_component_acl(entity, "")});

        snapshot_ostream->WriteEntity(current_entity_id, entity);
    }
}

void SnapshotGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("new_snapshot", "absolute_path_to_snapshot"), &SnapshotGenerator::new_snapshot);
    ClassDB::bind_method(D_METHOD("add_auto_instantiable_entity", "node"), &SnapshotGenerator::add_auto_instantiable_entity);
    ClassDB::bind_method(D_METHOD("add_streaming_tile_map", "node"), &SnapshotGenerator::add_streaming_tile_map_entities);
}

SnapshotGenerator::SnapshotGenerator() {
    snapshot_in_progress = false;
}

SnapshotGenerator::~SnapshotGenerator() {
    if (snapshot_ostream != nullptr && snapshot_in_progress) {
        logger.info("Finished writing snapshot.");
        delete snapshot_ostream;
    }
}