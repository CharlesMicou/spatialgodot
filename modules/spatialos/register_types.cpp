/* register_types.cpp */

#include "register_types.h"
#include "core/class_db.h"
#include "spatialos.h"

void register_spatialos_types() { 
        ClassDB::register_class<Spatialos>();
}

void unregister_spatialos_types() {
   //nothing to do here
}