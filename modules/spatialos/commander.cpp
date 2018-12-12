#include "commander.h"
#include "editor_node.h"
#include "worker_logger.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include "component_view.h"
#include "spatial_util.h"
#include "schema_parser.h"
#include "component_registry.h"

WorkerLogger Commander::logger = WorkerLogger("commander");

int Commander::create_entity(Dictionary components, Vector2 local_position, String specific_worker, bool persistant) {
    if (!create_call) {
            return 0;
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

    worker::RequestId<worker::CreateEntityRequest> req = create_call(entity);
    pending_creates.insert({{req, true}});
    return req.Id;
}

int Commander::delete_entity(std::int64_t entity_id) {
    if (!delete_call) {
            return 0;
    }
    worker::RequestId<worker::DeleteEntityRequest> req = delete_call(entity_id);
    pending_deletes.insert({{req, true}});
    return req.Id;
}

void Commander::handle_create_response(const worker::CreateEntityResponseOp& op) {
    auto it = pending_creates.find(op.RequestId);
    if (it == pending_creates.end()) {
        logger.warn("Received an response for unknown create entity request id");
        return;
    }
    if (op.StatusCode != worker::StatusCode::kSuccess) {
        logger.warn("Entity creation request failed " + op.Message);
        emit_signal("create_response", op.RequestId.Id, false, 0);
    } else {
        emit_signal("create_response", op.RequestId.Id, true, *op.EntityId);
    }
    pending_creates.erase(it->first);
}

void Commander::handle_delete_response(const worker::DeleteEntityResponseOp& op) {
    auto it = pending_deletes.find(op.RequestId);
    if (it == pending_deletes.end()) {
        logger.warn("Received an response for unknown delete entity request id");
        return;
    }
    if (op.StatusCode != worker::StatusCode::kSuccess) {
        logger.warn("Entity deletion request failed " + op.Message);
        emit_signal("delete_response", op.RequestId.Id, false, 0);
    } else {
        emit_signal("delete_response", op.RequestId.Id, true, op.EntityId);
    }
    pending_deletes.erase(it->first);
}

void Commander::init_callbacks(std::function<worker::RequestId<worker::CreateEntityRequest>(const worker::Entity& entity)> create_cb, 
        std::function<worker::RequestId<worker::DeleteEntityRequest>(const worker::EntityId entity_id)> delete_cb) {
            create_call = create_cb;
            delete_call = delete_cb;
}

Commander::Commander() {

}
