#ifndef SPOS_STREAMING_TILEMAP_H
#define SPOS_STREAMING_TILEMAP_H

#include "editor_node.h"
#include "scene/2d/tile_map.h"
#include "worker_logger.h"
#include "world_view.h"
#include "entity_view.h"
#include <improbable/worker.h>
#include <godotcore/godot_position2d.h>
#include <godotcore/tile_map_chunk.h>

class StreamingTileMap : public TileMap {
    GDCLASS(StreamingTileMap, TileMap);

    static WorkerLogger logger;
    worker::Map<worker::EntityId, std::list<std::pair<int, int>>> tracked_chunks;
    Vector2 godot_coordinates_to_grid_coordinates(const godotcore::GodotCoordinates2D& non_grid);
    godotcore::GodotCoordinates2D nearest_entity(Vector2 non_grid_pos);
    godotcore::TileCoordinate relative_coordinate(const godotcore::GodotCoordinates2D& entity, Vector2 non_grid_pos);

    protected:
        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("_on_entity_added", "entity_view"), &StreamingTileMap::_on_entity_added);
            ClassDB::bind_method(D_METHOD("_on_entity_removed", "entity_view"), &StreamingTileMap::_on_entity_removed);
        }

    public:
        void _on_entity_added(Node* added);
        void _on_entity_removed(Node* removed);
        std::list<worker::Entity> to_snapshot_entities();

        StreamingTileMap();
};
#endif
