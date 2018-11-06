#include "core/ustring.h"
#include "spatial_util.h"
#include <godotcore/godot_position2d.h>
#include <improbable/standard_library.h>
#include <string>

const int kChunkSizeInGodotUnits = 10000;
const float kGodotUnitsPerSpatialUnits = 20;

std::pair<int, float> toChunkedCoordinate(double unchunked) {
    int chunk = (int) (unchunked / kChunkSizeInGodotUnits);
    double local = unchunked - chunk * kChunkSizeInGodotUnits;
    return std::pair<int, float>(0, (float) local);
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

improbable::PositionData fromGodotPosition(const godotcore::GodotPosition2DData& godotPosition) {
    double spatial_x = (godotPosition.global_chunk().x() * kChunkSizeInGodotUnits + godotPosition.local_position().x()) / kGodotUnitsPerSpatialUnits;
    double spatial_z = - (godotPosition.global_chunk().y() * kChunkSizeInGodotUnits + godotPosition.local_position().y()) / kGodotUnitsPerSpatialUnits;
    return improbable::PositionData(improbable::Coordinates(spatial_x, 0.0, spatial_z));
}

godotcore::GodotPosition2DData toGodotPosition(const improbable::PositionData& improbablePosition) {
    std::pair<int, float> x = toChunkedCoordinate(improbablePosition.coords().x() * kGodotUnitsPerSpatialUnits);
    std::pair<int, float> y = toChunkedCoordinate(- improbablePosition.coords().z() * kGodotUnitsPerSpatialUnits);
    return godotcore::GodotPosition2DData(godotcore::GodotChunk2D(x.first, y.first), godotcore::GodotVector2D(x.second, y.second), godotcore::GodotVector2D{});
}
