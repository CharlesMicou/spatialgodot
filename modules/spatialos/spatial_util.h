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
improbable::PositionData fromGodotPosition(const godotcore::GodotPosition2DData& godotPosition);
godotcore::GodotPosition2DData toGodotPosition(const improbable::PositionData& improbablePosition);

#endif