#include "spatialos.h"
#include "core/ustring.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <iostream>
#include "editor_node.h"
#include "spatial_util.h"

using ComponentRegistry = worker::Components<improbable::Position, improbable::Metadata, godotcore::GodotPosition2D>;

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
    dispatcher->OnAuthorityChange<Metaclass>([&](const worker::AuthorityChangeOp& op) {
        worker::ComponentId componentId = Metaclass::ComponentId;
        world_view->authorityChange(op.EntityId, componentId, op.Authority);
    });
    dispatcher->OnAddComponent<Metaclass>([&](const worker::AddComponentOp<Metaclass>& op) {
        world_view->addComponent<Metaclass>(op);
    });
    dispatcher->OnRemoveComponent<Metaclass>([&](const worker::RemoveComponentOp& op) {
        worker::ComponentId componentId = Metaclass::ComponentId;
        world_view->removeComponent(op.EntityId, componentId);
    });
    dispatcher->OnComponentUpdate<Metaclass>([&](const worker::ComponentUpdateOp<Metaclass>& op) {
        world_view->updateComponent<Metaclass>(op);
    });
}

void Spatialos::setupDispatcher() {
    connection->SendLogMessage(worker::LogLevel::kInfo, "godot_core", "Hello from Godot!");
    dispatcher.reset(new worker::Dispatcher{ComponentRegistry{}});
    isConnected = true;

    // Super hacky but hey
    NodePath path = NodePath("WorldView");
    world_view = dynamic_cast<WorldView*>(get_node(path));

    // World view connections
    dispatcher->OnAddEntity([&](const worker::AddEntityOp& op) {
        world_view->addEntity(op);
    });
    dispatcher->OnRemoveEntity([&](const worker::RemoveEntityOp& op) {
        world_view->removeEntity(op);
    });

    // Components
    setupDispatcherForComponentMetaclass<improbable::Position>();

    // Todo: command responses
    // Todo: command requests
    // Todo: log messages, flags, queries, and all the others.

    // Misc. Dispatcher
    dispatcher->OnDisconnect([&](const worker::DisconnectOp& op) {
        std::cerr << "[disconnect] " << op.Reason << std::endl;
        isConnected = false;
    });
}

void Spatialos::processOps() {
    if (!isConnected) {
        return;
    }
    dispatcher->Process(connection->GetOpList(kOpsPerFrame));
}

void Spatialos::setPosition(std::int64_t entityId, double x, double y) {
    if (!isConnected) {
        return;
    }
    worker::EntityId oEntityId = entityId;
    godotcore::GodotPosition2DData asGodotData = godotcore::GodotPosition2DData(godotcore::GodotChunk2D(), godotcore::GodotVector2D(x, y), godotcore::GodotVector2D());
    connection->SendComponentUpdate<improbable::Position>(
        oEntityId, improbable::Position::Update{}.FromInitialData(fromGodotPosition(asGodotData)));
}

template <class ComponentUpdate>
void Spatialos::sendComponentUpdate(const worker::EntityId entity_id, const ComponentUpdate& update) {
    if (!isConnected) {
        return;
    }
    connection->SendComponentUpdate<ComponentUpdate>(entity_id, update);
}

void Spatialos::sendInfoMessage(const String &msg) {
    if (!isConnected) {
        return;
    }
    connection->SendLogMessage(worker::LogLevel::kInfo, "godot_user", fromGodotString(msg));
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
    ClassDB::bind_method(D_METHOD("connect_receptionist", "receptionist_host", "receptionist_port", "worker_id", "worker_type"), &Spatialos::blockingConnectReceptionist);
    ClassDB::bind_method(D_METHOD("connect_locator", "worker_type", "deployment_name", "project_name", "login_token"), &Spatialos::blockingConnectLocator);
    ClassDB::bind_method(D_METHOD("process_ops"), &Spatialos::processOps);
    ClassDB::bind_method(D_METHOD("set_position", "entityId", "x", "y"), &Spatialos::setPosition);
    ClassDB::bind_method(D_METHOD("send_log", "msg"), &Spatialos::sendInfoMessage);
}

Spatialos::Spatialos() {
    workerId = "defaultworkerid";
    workerType = "defaultworkertype";
    isConnected = false;
}
