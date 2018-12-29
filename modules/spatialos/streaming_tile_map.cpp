
#include "editor_node.h"
#include "worker_logger.h"
#include "streaming_tile_map.h"
#include "world_view.h"
#include "entity_view.h"
#include "component_view.h"
#include <improbable/worker.h>
#include <godotcore/tile_map_chunk.h>
#include <godotcore/godot_position2d.h>
#include "spatial_util.h"
#include <math.h>

WorkerLogger StreamingTileMap::logger = WorkerLogger("streaming_tile_map");
static const int streaming_chunk_edge_length = 8;

void StreamingTileMap::_on_entity_added(Node* added) {
    EntityView* entity_view = dynamic_cast<EntityView*>(added);
    if (entity_view == nullptr) {
        logger.error("Received an entity added that was not an entity view node");
        return;
    }
    if (!entity_view->has_component(godotcore::TileMapChunk::ComponentId)) {
        // We only care about TileMapChunk entities here
        return;
    }
    if (!entity_view->has_component(godotcore::GodotPosition2D::ComponentId)) {
        logger.error("The entity " + std::to_string(entity_view->entity_id) + " has a tile map chunk component, but no godot position.");
        return;
    }

    ComponentView<godotcore::GodotPosition2D>* position_node = dynamic_cast<ComponentView<godotcore::GodotPosition2D>*>(entity_view->getComponentNode(godotcore::GodotPosition2D::ComponentId));
    godotcore::GodotCoordinates2D coords = position_node->getData().coordinates();
    Vector2 coordinate_offset = godot_coordinates_to_grid_coordinates(coords);
    
    ComponentView<godotcore::TileMapChunk>* tile_map_node = dynamic_cast<ComponentView<godotcore::TileMapChunk>*>(entity_view->getComponentNode(godotcore::TileMapChunk::ComponentId));

    std::list<std::pair<int, int>> tiles_in_chunk;
    for (auto it : tile_map_node->getData().tile_ids()) {
        Vector2 target = coordinate_offset + Vector2(it.first.x(), it.first.y());
        set_cellv(target, it.second);
        tiles_in_chunk.push_back({target.x, target.y});
    }
    
    tracked_chunks.insert({{entity_view->entity_id, tiles_in_chunk}});
}

void StreamingTileMap::_on_entity_removed(Node* removed) {
    EntityView* entity_view = dynamic_cast<EntityView*>(removed);
    if (entity_view == nullptr) {
        logger.error("Received an entity removed that was not an entity view node");
        return;
    }
    auto it = tracked_chunks.find(entity_view->entity_id);
    if (it == tracked_chunks.end()) {
        return;
    }

    // Setting cells to -1 is equivalent to clearing them according to the godot docs
    for (auto tile : it->second) {
        set_cellv(Vector2(tile.first, tile.second), -1);
    }

    tracked_chunks.erase(it->first);
}

godotcore::GodotCoordinates2D StreamingTileMap::nearest_entity(Vector2 non_grid_pos) {
    int entity_grid_x  = non_grid_pos.x / (get_cell_size().x * streaming_chunk_edge_length);
    int entity_grid_y  = non_grid_pos.y / (get_cell_size().y * streaming_chunk_edge_length);
    return toGlobalGodotPosition(Vector2(
        entity_grid_x * get_cell_size().x * streaming_chunk_edge_length,
        entity_grid_y * get_cell_size().y * streaming_chunk_edge_length),
        0, 0);
}

godotcore::TileCoordinate StreamingTileMap::relative_coordinate(const godotcore::GodotCoordinates2D& entity, Vector2 non_grid_pos) {
    godotcore::TileCoordinate result;
    std::pair<float, float> localised = toLocalGodotPosition(entity, 0, 0);
    result.set_x((non_grid_pos.x - localised.first) / get_cell_size().x);
    result.set_y((non_grid_pos.y - localised.second) / get_cell_size().y);
    return result;
}

std::list<worker::Entity> StreamingTileMap::to_snapshot_entities() {
    worker::Map<godotcore::GodotCoordinates2D, worker::Map<godotcore::TileCoordinate, int32_t>> chunked;
    logger.info("Converting a tilemap with tiles of sizes " + std::to_string((int) get_cell_size().x) + " by " + std::to_string((int) get_cell_size().y) + " with chunked size " + std::to_string(streaming_chunk_edge_length));

    Array cells = get_used_cells();
    for (int i = 0; i < cells.size(); i++) {
        Vector2 grid_position = (Vector2)cells[i];
        Vector2 non_grid_pos = map_to_world(grid_position);
        godotcore::GodotCoordinates2D nearest_entity_position = nearest_entity(non_grid_pos);
        if (chunked.find(nearest_entity_position) == chunked.end()) {
            chunked.insert({{nearest_entity_position, {}}});
        }
        auto chunk_data = chunked.find(nearest_entity_position);
        chunk_data->second.insert({{relative_coordinate(nearest_entity_position, non_grid_pos), get_cellv(grid_position)}});
    }

    logger.info("Converted " + std::to_string(cells.size()) + " tile map cells to " + std::to_string(chunked.size()) + " entities.");

    std::list<worker::Entity> result;
    for (auto it : chunked) {
        worker::Entity entity;
        entity.Add<godotcore::GodotPosition2D>({it.first, {}});
        entity.Add<godotcore::TileMapChunk>({it.second});
        result.push_front(entity);
    }

    return result;
}

Vector2 StreamingTileMap::godot_coordinates_to_grid_coordinates(const godotcore::GodotCoordinates2D& non_grid) {
    std::pair<float, float> local = toLocalGodotPosition(non_grid, 0, 0);
    return world_to_map(Vector2(local.first, local.second));
}

StreamingTileMap::StreamingTileMap() {
}
