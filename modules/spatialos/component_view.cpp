#include "component_view.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>

template <class T>
void ComponentView<T>::_bind_methods() {
}

template <class T>
void ComponentView<T>::authorityChange(const worker::Authority& authority) {

}

template <class T>
void ComponentView<T>::updateComponent(const worker::ComponentUpdateOp<T>& update) {

}

template <class T>
void ComponentView<T>::removeComponent() {

}

template <class T>
ComponentView<T>::ComponentView() {
}

// Force generation so that linking works
template class ComponentView<improbable::Position>;