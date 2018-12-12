#include "commander.h"
#include "editor_node.h"
#include "worker_logger.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include "component_view.h"
#include "spatial_util.h"
#include "schema_parser.h"
#include "spatialos.h"
#include "component_registry.h"

worker::Map<worker::ComponentId, worker::detail::ComponentMetaclass> m;

int Commander::create_entity(Dictionary components, Vector2 local_position, String specific_worker, bool persistant) {
    if (spos == nullptr) {
            // not ready to send yet
            return false;
    }

    worker::Entity entity;
    SchemaParser::applyComponentsToEntity(entity, components);
    godotcore::GodotCoordinates2D position = toGlobalGodotPosition(local_position, 0, 0);
    entity.Add<godotcore::GodotPosition2D>({position, {}});
    entity.Add<improbable::Position>(fromGodotPosition(position));
    if (persistant) {
        entity.Add<improbable::Persistence>({});
    }
    entity.Add<improbable::EntityAcl>({clientAndServerReqSet, make_component_acl(entity, fromGodotString(specific_worker))});

    Spatialos* s = dynamic_cast<Spatialos*>(spos);
    if (s == nullptr) {
        logger.error("Commander was not able to locate Spatialos node");
    }

    worker::RequestId<worker::CreateEntityRequest> req = s->sendCreateCommand(entity);
    pending_creates.insert({{req, true}});
    return req.Id;
}

int Commander::delete_entity(std::int64_t entity_id) {

}

