#include "world_view.h"
#include <improbable/worker.h>
#include <iostream>

void WorldView::_bind_methods() {
}

void WorldView::addEntity(const worker::AddEntityOp& add) {
    std::cout << "Received an add entity" << std::endl;
}

void WorldView::removeEntity(const worker::RemoveEntityOp& remove) {
    std::cout << "Received a remove entity" << std::endl;
}

void WorldView::authorityChange(const worker::AuthorityChangeOp& authority) {

}

template <typename T>
void WorldView::addComponent(const worker::AddComponentOp<T>& add) {

}

template <typename T>
void WorldView::updateComponent(const worker::ComponentUpdateOp<T>& update) {

}

void WorldView::removeComponent(const worker::RemoveComponentOp& remove) {

}

void WorldView::handleCriticalSection(const worker::CriticalSectionOp& section) {
    inCriticalSection = section.InCriticalSection;
    if (!inCriticalSection) {
        // this is going to be interesting.
    }
}

WorldView::WorldView() {
}

WorldView::~WorldView() {
    std::cout << "Destroying world view" << std::endl;
}