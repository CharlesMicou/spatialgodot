#include "spatialos.h"
#include "core/ustring.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <godotcore/auto_instantiable.h>
#include <spellcrest/player_controls.h>
#include "component_registry.h"
#include "core/os/os.h"
#include "editor_node.h"
#include "spatial_util.h"
#include "auto_instantiator.h"

const std::string kLocatorHost = "locator.improbable.io";
const String kLogFileFlag = "--spatialos_log_file";

worker::Connection ConnectWithReceptionist(const std::string hostname,
                                           const std::uint16_t port,
                                           const std::string& worker_id,
                                           const worker::ConnectionParameters& connection_parameters) {
    auto future = worker::Connection::ConnectAsync(MergedComponentRegistry(), hostname, port, worker_id, connection_parameters);
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
        MergedComponentRegistry(),
        deployment_name,
        connection_parameters,
        [](worker::QueueStatus queueStatus) -> bool {return true;}); // Indefinitely retry the queue
    worker::Connection result = future.Get();
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
    logger.info("Trying to login in via receptionist.");
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
    logger.info("Trying to login in via locator.");
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

    WorkerLogger::init_remote_log_callback([&](const worker::LogLevel level, const std::string& name, const std::string& msg){
        connection->SendLogMessage(level, name, msg);
    }, log_severity::WARN /* send warn and above over the protocol */);

    dispatcher.reset(new worker::Dispatcher{MergedComponentRegistry()});
    isConnected = true;

    // Super hacky but hey
    NodePath world_view_path = NodePath("WorldView");
    world_view = dynamic_cast<WorldView*>(get_node(world_view_path));
    NodePath auto_instantiator_path = NodePath("AutoInstantiator");
    AutoInstantiator* auto_instantiator = dynamic_cast<AutoInstantiator*>(get_node(auto_instantiator_path));
    auto_instantiator->start(world_view);
    NodePath commander_path = NodePath("Commander");
    commander = dynamic_cast<Commander*>(get_node(commander_path));

    // Messages
    dispatcher->OnLogMessage([&](const worker::LogMessageOp& op) {
        logger.warn("[worker sdk] " + op.Message);
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
    setupDispatcherForComponentMetaclass<improbable::Metadata>();
    setupDispatcherForComponentMetaclass<godotcore::GodotPosition2D>();
    setupDispatcherForComponentMetaclass<godotcore::AutoInstantiable>();
    setupDispatcherForComponentMetaclass<spellcrest::PlayerControls>();
    setupDispatcherForComponentMetaclass<spellcrest::ChatParticipant>();

    // System commands
    dispatcher->OnReserveEntityIdResponse([&](const worker::ReserveEntityIdResponseOp op) {
        // currently there's no way to make these requests
    });
    dispatcher->OnCreateEntityResponse([&](const worker::CreateEntityResponseOp op) {
        commander->handle_create_response(op);
    });
    dispatcher->OnDeleteEntityResponse([&](const worker::DeleteEntityResponseOp op) {
        commander->handle_delete_response(op);
    });

    // Setup ability to send commands only after we're able to receive them
    commander->init_callbacks(
        [&](const worker::Entity entity_data){
            return connection->SendCreateEntityRequest(entity_data, {/* reserved id */}, {5000});
        },
        [&](const worker::EntityId entity_id){
            return connection->SendDeleteEntityRequest(entity_id, {5000});
        });

    // Todo: entity commands

    // Todo: query commands

    // Todo: read worker flags

    // Misc. Dispatcher
    dispatcher->OnDisconnect([&](const worker::DisconnectOp& op) {
        logger.warn("Disconnected with reason: " + op.Reason);
        isConnected = false;
        WorkerLogger::on_connection_closed();
        emit_signal("disconnected");
    });
    emit_signal("connection_success");
}

void Spatialos::processOps() {
    if (!isConnected) {
        return;
    }
    dispatcher->Process(connection->GetOpList(0));
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

worker::RequestId<worker::CreateEntityRequest> Spatialos::sendCreateCommand(const worker::Entity& entity) {
    return connection->SendCreateEntityRequest(entity, {}, {5000});
}

void Spatialos::initLogging() {
    #ifdef DEBUG_ENABLED
        WorkerLogger::set_console_severity(log_severity::INFO);
    #else
        WorkerLogger::set_console_severity(log_severity::MAX);
    #endif
    List<String> args = OS::get_singleton()->get_cmdline_args();
    auto it = args.front();
    while (it) {
            if (kLogFileFlag.is_subsequence_of(it->get())) {
                String dest = it->get().split("=")[1];
                WorkerLogger::init_log_file(fromGodotString(dest));
                // If we have a log file, we can turn off console logging.
                WorkerLogger::set_console_severity(log_severity::MAX);
                break;
            }
			it = it->next();
	};
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

String Spatialos::get_worker_id() {
    return workerId;
}

void Spatialos::debug_method(String arbitrary_input) {
    // Put debug in here
}

void Spatialos::_bind_methods() {
    ADD_SIGNAL(MethodInfo("connection_success"));
    ADD_SIGNAL(MethodInfo("disconnected"));

    ClassDB::bind_method(D_METHOD("connect_receptionist", "receptionist_host", "receptionist_port", "worker_id", "worker_type"), &Spatialos::blockingConnectReceptionist);
    ClassDB::bind_method(D_METHOD("connect_locator", "worker_type", "deployment_name", "project_name", "login_token"), &Spatialos::blockingConnectLocator);
    ClassDB::bind_method(D_METHOD("process_ops"), &Spatialos::processOps);
    ClassDB::bind_method(D_METHOD("send_log", "msg"), &Spatialos::sendInfoMessage);

    ClassDB::bind_method(D_METHOD("get_worker_id"), &Spatialos::get_worker_id);

    // Debug method for testing various features from gdscript
    ClassDB::bind_method(D_METHOD("debug_method"), &Spatialos::debug_method);
}

Spatialos::Spatialos(): logger(WorkerLogger("core")) {
    workerId = "defaultworkerid";
    workerType = "defaultworkertype";
    isConnected = false;
    initLogging();
}

template void Spatialos::sendComponentUpdate<improbable::Position>(const worker::EntityId entity_id, const improbable::Position::Update& update);
template void Spatialos::sendComponentUpdate<improbable::Metadata>(const worker::EntityId entity_id, const improbable::Metadata::Update& update);
template void Spatialos::sendComponentUpdate<godotcore::GodotPosition2D>(const worker::EntityId entity_id, const godotcore::GodotPosition2D::Update& update);
template void Spatialos::sendComponentUpdate<godotcore::AutoInstantiable>(const worker::EntityId entity_id, const godotcore::AutoInstantiable::Update& update);
template void Spatialos::sendComponentUpdate<spellcrest::PlayerControls>(const worker::EntityId entity_id, const spellcrest::PlayerControls::Update& update);
template void Spatialos::sendComponentUpdate<spellcrest::ChatParticipant>(const worker::EntityId entity_id, const spellcrest::ChatParticipant::Update& update);
