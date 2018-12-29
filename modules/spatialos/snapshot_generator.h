#ifndef SNAPSHOT_GENERATOR_H
#define SNAPSHOT_GENERATOR_H

#include "editor_node.h"
#include "improbable/worker.h"
#include "worker_logger.h"

class SnapshotGenerator : public Node {
    private:
        GDCLASS(SnapshotGenerator, Node);
        static WorkerLogger logger;
        worker::SnapshotOutputStream* snapshot_ostream;
        std::uint64_t current_entity_id;
        bool snapshot_in_progress;

    protected:
        static void _bind_methods();

    public:
        void new_snapshot(String path);
        void add_auto_instantiable_entity(Node* node);
        void add_streaming_tile_map_entities(Node* node);
        SnapshotGenerator();
        ~SnapshotGenerator();
};

#endif