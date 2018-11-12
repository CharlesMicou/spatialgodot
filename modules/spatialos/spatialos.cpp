#include "spatialos.h"
#include "core/ustring.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <iostream>
#include "editor_node.h"
#include "spatial_util.h"

using ComponentRegistry = worker::Components<improbable::Position, improbable::Metadata, godotcore::GodotPosition2D, improbable::EntityAcl, improbable::Persistence>;

const std::string kLocatorHost = "locator.improbable.io";
const int kOpsPerFrame = 1000;

worker::Connection ConnectWithReceptionist(const std::string hostname,
                                           const std::uint16_t port,
                                           const std::string& worker_id,
                                           const worker::ConnectionParameters& connection_parameters) {
    auto future = worker::Connection::ConnectAsync(ComponentRegistry{}, hostname, port, worker_id, connection_parameters);
    return future.Get();
}

worker::Connection ConnectWithLocator(
    const std::string& deployment_name,
    const std::string& project_name,
    const std::string& login_token,
    const worker::ConnectionParameters& connection_parameters) {
    worker::LocatorParameters locatorParams;
    locatorParams.LoginToken = worker::LoginTokenCredentials{login_token};
    locatorParams.EnableLogging = false;
    locatorParams.ProjectName = project_name;
    locatorParams.CredentialsType = worker::LocatorCredentialsType::kLoginToken;
    worker::Locator locator = worker::Locator(kLocatorHost, locatorParams);
    auto future = locator.ConnectAsync(
        ComponentRegistry{},
        deployment_name,
        connection_parameters,
        [](worker::QueueStatus queueStatus) -> bool {return true;}); // Indefinitely retry the queue
    std::cout << "Blocking on locator connection." << std::endl;
    worker::Connection result = future.Get();
    std::cout << "Locator result complete." << std::endl;
    return result;
}

void Spatialos::blockingConnectReceptionist(
    const String &receptionistIp,
    const int receptionistPort,
    const String &id,
    const String &type) {
    workerId = id;
    workerType = type;
    worker::ConnectionParameters parameters;
    parameters.WorkerType = fromGodotString(workerType);
    parameters.Network.ConnectionType = worker::NetworkConnectionType::kTcp;
    parameters.Network.UseExternalIp = false;

    connection.reset(new worker::Connection{ConnectWithReceptionist(fromGodotString(receptionistIp), receptionistPort, fromGodotString(workerId), parameters)});

    setupDispatcher();
}

void Spatialos::blockingConnectLocator(
    const String &type,
    const String &dplName,
    const String &projectName,
    const String &loginToken) {
    workerType = type;
    worker::ConnectionParameters parameters;
    parameters.WorkerType = fromGodotString(workerType);
    parameters.Network.ConnectionType = worker::NetworkConnectionType::kTcp;
    parameters.Network.UseExternalIp = true;
    connection.reset(new worker::Connection{ConnectWithLocator(fromGodotString(dplName), fromGodotString(projectName), fromGodotString(loginToken), parameters)});
    workerId = toGodotString(connection->GetWorkerId());
    setupDispatcher();
}

template <typename Metaclass>
void Spatialos::setupDispatcherForComponentMetaclass() {
    dispatcher->OnAddComponent<Metaclass>([&](const worker::AddComponentOp<Metaclass>& op) {
        world_view->addComponent<Metaclass>(op);
    });
    dispatcher->OnAuthorityChange<Metaclass>([&](const worker::AuthorityChangeOp& op) {
        worker::ComponentId componentId = Metaclass::ComponentId;
        world_view->authorityChange(op.EntityId, componentId, op.Authority);
    });
    dispatcher->OnComponentUpdate<Metaclass>([&](const worker::ComponentUpdateOp<Metaclass>& op) {
        world_view->updateComponent<Metaclass>(op);
    });
    dispatcher->OnRemoveComponent<Metaclass>([&](const worker::RemoveComponentOp& op) {
        worker::ComponentId componentId = Metaclass::ComponentId;
        world_view->removeComponent(op.EntityId, componentId);
    });
}

void Spatialos::setupDispatcher() {
    connection->SendLogMessage(worker::LogLevel::kInfo, "godot_core", "Hello from Godot!");
    dispatcher.reset(new worker::Dispatcher{ComponentRegistry{}});
    isConnected = true;

    // Super hacky but hey
    NodePath path = NodePath("WorldView");
    world_view = dynamic_cast<WorldView*>(get_node(path));

    // Messages
    dispatcher->OnLogMessage([&](const worker::LogMessageOp& op) {
        std::cout << "[worker sdk]: " << op.Message << std::endl;
    });

    // World view connections
    dispatcher->OnCriticalSection([&](const worker::CriticalSectionOp& op) {
        world_view->handleCriticalSection(op);
    });
    dispatcher->OnAddEntity([&](const worker::AddEntityOp& op) {
        world_view->addEntity(op);
    });
    dispatcher->OnRemoveEntity([&](const worker::RemoveEntityOp& op) {
        world_view->removeEntity(op);
    });

    // Components
    setupDispatcherForComponentMetaclass<improbable::Position>();
    
    // Note(charlie): components need to be properly generic and not have position hacks first
    //setupDispatcherForComponentMetaclass<improbable::Metadata>();

    // Todo: command responses
    // Todo: command requests
    // Todo: log messages, flags, queries, and all the others.
    dispatcher->OnReserveEntityIdResponse([&](const worker::ReserveEntityIdResponseOp op) {
        if (op.StatusCode != worker::StatusCode::kSuccess) {
            std::cout << "Received a reserve entity failure: " + op.Message << std::endl;
            return;
        }
        if (op.EntityId.empty()) {
            std::cout << "Reserve entity was a success but also empty"<< std::endl;
            return;
        }

        int entityId = *op.EntityId;
        std::cout << "Reserved an entity " << std::to_string(*op.EntityId) << " " << std::to_string(entityId) << std::endl;
        emit_signal("entity_reserved", entityId);
    });
    dispatcher->OnCreateEntityResponse([&](const worker::CreateEntityResponseOp op) {
        if (op.StatusCode != worker::StatusCode::kSuccess) {
            std::cout << "Received a create entity failure: " + op.Message << std::endl;
        } else {
            std::cout << "Received a create entity success" << std::endl;
        }
    });

    // Misc. Dispatcher
    dispatcher->OnDisconnect([&](const worker::DisconnectOp& op) {
        std::cerr << "[disconnect] " << op.Reason << std::endl;
        isConnected = false;
    });
    emit_signal("connection_success");
}

void Spatialos::processOps() {
    if (!isConnected) {
        return;
    }
    dispatcher->Process(connection->GetOpList(kOpsPerFrame));
}

template <typename T>
void Spatialos::sendComponentUpdate(const worker::EntityId entity_id, const typename T::Update& update) {
    if (!isConnected) {
        return;
    }
    connection->SendComponentUpdate<T>(entity_id, update);
}

void Spatialos::sendInfoMessage(const String &msg) {
    if (!isConnected) {
        return;
    }
    connection->SendLogMessage(worker::LogLevel::kInfo, "godot_user", fromGodotString(msg));
}

void Spatialos::spawnPlayerEntity(int entity_id) {
    std::cout << "Attempting to spawn entity " << std::to_string(entity_id) << std::endl;
    worker::EntityId entityId = entity_id;
    worker::Entity entityToSpawn;
    entityToSpawn.Add<improbable::Position>({{0, 0, 0}});
    entityToSpawn.Add<improbable::Metadata>({"Client"});

    improbable::WorkerRequirementSet bothReqSet{{{{"gserver"}}, {{"gclient"}}}};
    //improbable::WorkerRequirementSet bothReqSet{{{{"gserver"}}}};
    std::string hackyAttribute = "workerId:" + fromGodotString(workerId);
    worker::Map<worker::ComponentId, improbable::WorkerRequirementSet> component_acl = {{improbable::Position::ComponentId, {{{{hackyAttribute}}}}}};
    entityToSpawn.Add<improbable::EntityAcl>({bothReqSet, component_acl});

    connection->SendCreateEntityRequest(entityToSpawn, entityId, {5000} /* timeout */);
}

void Spatialos::reserveId() {
    std::cout << "Attempting to reserve an entity id" << std::endl;
    connection->SendReserveEntityIdRequest({} /* timeout */);
}

String Spatialos::get_configuration_warning() const {
	String warning = Node::get_configuration_warning();
	if (world_view == NULL) {
		if (warning == String()) {
			warning += "\n";
		}
		warning += TTR("This node requires a WorldView child to work properly.");
	}
	return warning;
}

void Spatialos::_bind_methods() {
    ADD_SIGNAL(MethodInfo("connection_success"));

    ClassDB::bind_method(D_METHOD("connect_receptionist", "receptionist_host", "receptionist_port", "worker_id", "worker_type"), &Spatialos::blockingConnectReceptionist);
    ClassDB::bind_method(D_METHOD("connect_locator", "worker_type", "deployment_name", "project_name", "login_token"), &Spatialos::blockingConnectLocator);
    ClassDB::bind_method(D_METHOD("process_ops"), &Spatialos::processOps);
    ClassDB::bind_method(D_METHOD("send_log", "msg"), &Spatialos::sendInfoMessage);

    // Hacky signals until a commander is implemented
    ADD_SIGNAL(MethodInfo("entity_reserved", PropertyInfo(Variant::INT, "reserved_entity_id")));
    ClassDB::bind_method(D_METHOD("spawn_entity", "entity_id"), &Spatialos::spawnPlayerEntity);
    ClassDB::bind_method(D_METHOD("reserve_id"), &Spatialos::reserveId);
}

Spatialos::Spatialos() {
    workerId = "defaultworkerid";
    workerType = "defaultworkertype";
    isConnected = false;
}

template void Spatialos::sendComponentUpdate<improbable::Position>(const worker::EntityId entity_id, const improbable::Position::Update& update);
//template void Spatialos::sendComponentUpdate<improbable::Metadata>(const worker::EntityId entity_id, const improbable::Metadata::Update& update);
