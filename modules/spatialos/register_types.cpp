/* register_types.cpp */

#include "register_types.h"
#include "core/class_db.h"
#include "spatialos.h"
#include "world_view.h"
#include "entity_view.h"
#include "component_view.h"
#include <improbable/standard_library.h>

void register_spatialos_types() { 
        ClassDB::register_class<Spatialos>();
        ClassDB::register_class<WorldView>();
        ClassDB::register_class<EntityView>();

        // Explicitly declare schema stuff here
        ClassDB::register_class<ComponentView<improbable::Position>>();
}

void unregister_spatialos_types() {
   //nothing to do here
}