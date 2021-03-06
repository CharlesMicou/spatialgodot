#include "core/ustring.h"
#include "spatial_util.h"
#include "editor_node.h"
#include <godotcore/godot_position2d.h>
#include <improbable/standard_library.h>
#include <string>

const int kChunkSizeInGodotUnits = 10000;
const float kGodotUnitsPerSpatialUnits = 20;

std::pair<int, float> toChunkedCoordinate(double unchunked) {
    int chunk = (int) (unchunked / kChunkSizeInGodotUnits);
    double local = unchunked - chunk * kChunkSizeInGodotUnits;
    return std::pair<int, float>(chunk, (float) local);
}

std::string fromGodotString(const String& godotString) {
    std::wstring wide(godotString.ptr());
    std::string s(wide.begin(), wide.end());
    return s;
}

String toGodotString(const std::string& regularString) {
    String s(regularString.c_str());
    return s;
}

improbable::Coordinates fromGodotPosition(const godotcore::GodotCoordinates2D& godotPosition) {
    double spatial_x = (godotPosition.global_chunk().x() * kChunkSizeInGodotUnits + godotPosition.local_position().x()) / kGodotUnitsPerSpatialUnits;
    double spatial_z = - (godotPosition.global_chunk().y() * kChunkSizeInGodotUnits + godotPosition.local_position().y()) / kGodotUnitsPerSpatialUnits;
    return improbable::Coordinates(spatial_x, 0.0, spatial_z);
}

godotcore::GodotCoordinates2D toGodotPosition(const improbable::PositionData& improbablePosition) {
    std::pair<int, float> x = toChunkedCoordinate(improbablePosition.coords().x() * kGodotUnitsPerSpatialUnits);
    std::pair<int, float> y = toChunkedCoordinate(- improbablePosition.coords().z() * kGodotUnitsPerSpatialUnits);
    return godotcore::GodotCoordinates2D(godotcore::GodotChunk2D(x.first, y.first), godotcore::GodotVector2D(x.second, y.second));
}

std::pair<float, float> toLocalGodotPosition(const godotcore::GodotCoordinates2D& godotPosition, const int x_chunk_origin, const int y_chunk_origin) {
    int relative_x_chunk = godotPosition.global_chunk().x() - x_chunk_origin;
    float local_x = relative_x_chunk * kChunkSizeInGodotUnits + godotPosition.local_position().x();
    int relative_y_chunk = godotPosition.global_chunk().y() - y_chunk_origin;
    float local_y = relative_y_chunk * kChunkSizeInGodotUnits + godotPosition.local_position().y();
    return std::pair<float, float>(local_x, local_y);
}

godotcore::GodotCoordinates2D toGlobalGodotPosition(const Vector2 local_position, const int x_chunk_origin, const int y_chunk_origin) {
    return godotcore::GodotCoordinates2D(godotcore::GodotChunk2D(x_chunk_origin, y_chunk_origin), godotcore::GodotVector2D(local_position.x, local_position.y));
}