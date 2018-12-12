#ifndef SPATIAL_UTIL_H
#define SPATIAL_UTIL_H

#include "core/ustring.h"
#include <godotcore/godot_position2d.h>
#include <improbable/standard_library.h>
#include <string>

// String conversions
std::string fromGodotString(const String& godotString);
String toGodotString(const std::string& regularString);

// Position conversions
improbable::Coordinates fromGodotPosition(const godotcore::GodotCoordinates2D& godotPosition);
godotcore::GodotCoordinates2D toGodotPosition(const improbable::PositionData& improbablePosition);
std::pair<float, float> toLocalGodotPosition(const godotcore::GodotCoordinates2D& godotPosition, const int x_chunk_origin, const int y_chunk_origin);
godotcore::GodotCoordinates2D toGlobalGodotPosition(const Vector2 local_position, const int x_chunk_origin, const int y_chunk_origin);

#endif