#include "component_view.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>
#include "spatial_util.h"
#include <stdio.h>

const worker::ComponentId kPositionId = 54;

template <typename T>
void ComponentView<T>::_bind_methods() {
    // Note(charlie): literally just refs until I figure this out. could also be a void.
    ADD_SIGNAL(MethodInfo("component_updated", PropertyInfo(Variant::OBJECT, "component_update", PROPERTY_HINT_NONE, "Reference")));
    ADD_SIGNAL(MethodInfo("authority_changed", PropertyInfo(Variant::BOOL, "authority")));

    // Yolo position hacks while flows get figured out
    ADD_SIGNAL(MethodInfo("yolo_position_update", PropertyInfo(Variant::REAL, "x"), PropertyInfo(Variant::REAL, "y")));
    ClassDB::bind_method(D_METHOD("get_position"), &ComponentView::getPosition);
}

template <typename T>
void ComponentView<T>::authorityChange(const worker::Authority& authority) {
    bool prev = authoritative;
    authoritative = (authority == worker::Authority::kAuthoritative);
    if (authoritative != prev) {
        emit_signal("authority_changed", authoritative);
    }
}

template <typename T>
void ComponentView<T>::updateComponent(const worker::ComponentUpdateOp<T>& update) {
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

template <typename T>
void ComponentView<T>::init(const worker::ComponentId component_id, const typename T::Data& state) {
    // only works for position for now
    if (component_id != 54) {
        return;
    }
    data = state;
    const improbable::Coordinates& coords = state.coords();
    godotcore::GodotPosition2DData global = toGodotPosition(coords);
    std::pair<float, float> local_positon = toLocalGodotPosition(global, 0, 0);
    syncedPos.x = local_positon.first;
    syncedPos.y = local_positon.second;
}

template <typename T>
Vector2 ComponentView<T>::getPosition() {
    return syncedPos;
}

template <typename T>
ComponentView<T>::ComponentView() {
    authoritative = false;
}

template <typename T>
const bool ComponentView<T>::hasAuthority() {
    return authoritative;
}

// Force generation so that linking works
template class ComponentView<improbable::Position>;
//template class ComponentView<improbable::Metadata>;