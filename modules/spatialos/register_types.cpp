/* register_types.cpp */

#include "register_types.h"
#include "core/class_db.h"
#include "spatialos.h"
#include "world_view.h"

void register_spatialos_types() { 
        ClassDB::register_class<Spatialos>();
        ClassDB::register_class<WorldView>();
}

void unregister_spatialos_types() {
   //nothing to do here
}