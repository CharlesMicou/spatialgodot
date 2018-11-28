#ifndef SNAPSHOT_GENERATOR_H
#define SNAPSHOT_GENERATOR_H

#include "editor_node.h"

class SnapshotGenerator : public Node {
    private:
        GDCLASS(SnapshotGenerator, Node);

    protected:
        static void _bind_methods();

    public:
        void takeSnapshot(String path);
        SnapshotGenerator();
};

#endif