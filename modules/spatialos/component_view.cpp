#include "component_view.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>

void ComponentView::_bind_methods() {
    // Note(charlie): literally just refs until I figure this out
    ADD_SIGNAL(MethodInfo("component_updated", PropertyInfo(Variant::OBJECT, "component_update", PROPERTY_HINT_RESOURCE_TYPE, "Reference")));
    ADD_SIGNAL(MethodInfo("authority_changed", PropertyInfo(Variant::BOOL, "authority")));
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
    // todo emit update signal

}


ComponentView::ComponentView() {
}

// Force generation so that linking works
template void ComponentView::updateComponent<improbable::Position>(const worker::ComponentUpdateOp<improbable::Position>&);