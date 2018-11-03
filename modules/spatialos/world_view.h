#ifndef WORLD_VIEW_H
#define WORLD_VIEW_H

#include "editor_node.h"

class WorldView : public Node2D {
    GDCLASS(WorldView, Node2D);

protected:
    static void _bind_methods();

public:
    void dummy();
    WorldView();
};

#endif