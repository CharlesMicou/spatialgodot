#include "component_view.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>
#include "spatial_util.h"
#include <stdio.h>

const worker::ComponentId kPositionId = 54;

void ComponentView::_bind_methods() {
    // Note(charlie): literally just refs until I figure this out. could also be a void.
    ADD_SIGNAL(MethodInfo("component_updated", PropertyInfo(Variant::OBJECT, "component_update", PROPERTY_HINT_NONE, "Reference")));
    ADD_SIGNAL(MethodInfo("authority_changed", PropertyInfo(Variant::BOOL, "authority")));

    // Yolo position hacks while flows get figured out
    ADD_SIGNAL(MethodInfo("yolo_position_update", PropertyInfo(Variant::REAL, "x"), PropertyInfo(Variant::REAL, "y")));
    ClassDB::bind_method(D_METHOD("get_position"), &ComponentView::getPosition);
}

void ComponentView::authorityChange(const worker::Authority& authority) {
    bool prev = authoritative;
    authoritative = (authority == worker::Authority::kAuthoritative);
    if (authoritative != prev) {
        emit_signal("authority_changed", authoritative);
    }
}

template <class T>
void ComponentView::updateComponent(const worker::ComponentUpdateOp<T>& update) {
    if (T::ComponentId == kPositionId) {
        // Note to self: this is an optional
        if (update.Update.coords()) {
            const improbable::Coordinates& coords = *update.Update.coords();
            godotcore::GodotPosition2DData global = toGodotPosition(coords);
            std::pair<float, float> local_positon = toLocalGodotPosition(global, 0, 0);
            syncedPos.x = local_positon.first;
            syncedPos.y = local_positon.second;
            emit_signal("yolo_position_update", local_positon.first, local_positon.second);
        }
    }
    // todo emit update signal once schema types are properly exposed
}

template <class DataT>
void ComponentView::populateComponent(const DataT& initial) {
    // only works for position for now
    const improbable::Coordinates& coords = initial.coords();
    godotcore::GodotPosition2DData global = toGodotPosition(coords);
    std::pair<float, float> local_positon = toLocalGodotPosition(global, 0, 0);
    syncedPos.x = local_positon.first;
    syncedPos.y = local_positon.second;
}

Vector2 ComponentView::getPosition() {
    return syncedPos;
}

ComponentView::ComponentView() {
}

// Force generation so that linking works
template void ComponentView::updateComponent<improbable::Position>(const worker::ComponentUpdateOp<improbable::Position>&);
template void ComponentView::populateComponent<improbable::Position::Data>(const improbable::Position::Data&);