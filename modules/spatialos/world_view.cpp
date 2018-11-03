#include "world_view.h"

void WorldView::_bind_methods() {
    ClassDB::bind_method(D_METHOD("dummy"), &WorldView::dummy);
}

void WorldView::dummy() {
}

WorldView::WorldView() {
}
