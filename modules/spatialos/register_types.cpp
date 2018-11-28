/* register_types.cpp */

#include "register_types.h"
#include "core/class_db.h"
#include "spatialos.h"
#include "world_view.h"
#include "entity_view.h"
#include "component_view.h"
#include "position_sync.h"
#include "worker_logger_wrapper.h"
#include "snapshot_generator.h"

void register_spatialos_types() { 
        ClassDB::register_class<Spatialos>();
        ClassDB::register_class<WorldView>();
        ClassDB::register_class<EntityView>();
        ClassDB::register_virtual_class<ComponentViewBase>();
        ClassDB::register_class<PositionSync>();
        ClassDB::register_class<WorkerLoggerWrapper>();
        ClassDB::register_class<SnapshotGenerator>();
}

void unregister_spatialos_types() {
   //nothing to do here
}
