#include "component_view.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include "spatial_util.h"
#include <stdio.h>

const worker::ComponentId kPositionId = 54;
template <typename T>
WorkerLogger ComponentView<T>::logger = WorkerLogger("component_view");

template <typename T>
void ComponentView<T>::_bind_methods() {
    // Note(charlie): literally just refs until I figure this out. could also be a void.
    ADD_SIGNAL(MethodInfo("component_updated", PropertyInfo(Variant::OBJECT, "component_update", PROPERTY_HINT_NONE, "Reference")));
    ADD_SIGNAL(MethodInfo("authority_changed", PropertyInfo(Variant::BOOL, "authority")));
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
    update.Update.ApplyTo(data);
}

template <typename T>
void ComponentView<T>::init(const worker::ComponentId component_id, const typename T::Data& state) {
    data = state;
}

template <typename T>
const typename T::Data& ComponentView<T>::getData() {
    return data;
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