#include "spatialos.h"
#include "core/ustring.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <godotcore/godot_position2d.h>
#include <iostream>

using ComponentRegistry = worker::Components<improbable::Position, improbable::Metadata, godotcore::GodotPosition2D>;

const std::string kLocatorHost = "locator.improbable.io";

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

std::string strConvert(const String &godotString) {
    std::wstring wide(godotString.ptr());
    std::string s(wide.begin(), wide.end());
    return s;
}

String toGodotString(const std::string &regularString) {
    String s(regularString.c_str());
    return s;
}

void Spatialos::blockingConnectReceptionist(
    const String &receptionistIp,
    const int receptionistPort,
    const String &id,
    const String &type) {
    workerId = id;
    workerType = type;
    worker::ConnectionParameters parameters;
    parameters.WorkerType = strConvert(workerType);
    parameters.Network.ConnectionType = worker::NetworkConnectionType::kTcp;
    parameters.Network.UseExternalIp = false;

    connection.reset(new worker::Connection{ConnectWithReceptionist(strConvert(receptionistIp), receptionistPort, strConvert(workerId), parameters)});

    postConnection();
}

void Spatialos::blockingConnectLocator(
    const String &type,
    const String &dplName,
    const String &projectName,
    const String &loginToken) {
    workerType = type;
    worker::ConnectionParameters parameters;
    parameters.WorkerType = strConvert(workerType);
    parameters.Network.ConnectionType = worker::NetworkConnectionType::kTcp;
    parameters.Network.UseExternalIp = true;
    connection.reset(new worker::Connection{ConnectWithLocator(strConvert(dplName), strConvert(projectName), strConvert(loginToken), parameters)});
    workerId = toGodotString(connection->GetWorkerId());
    postConnection();
}

void Spatialos::postConnection() {
    connection->SendLogMessage(worker::LogLevel::kInfo, "godot_core", "Hello from Godot!");
    dispatcher.reset(new worker::Dispatcher{ComponentRegistry{}});

    isConnected = true;
    dispatcher->OnDisconnect([&](const worker::DisconnectOp& op) {
        std::cerr << "[disconnect] " << op.Reason << std::endl;
        isConnected = false;
    });

    dispatcher->OnAddEntity([&](const worker::AddEntityOp& op) {
        worker::EntityId entityId = op.EntityId;
        std::string entityAddedMsg = "got entity add for ";
        entityAddedMsg += std::to_string(entityId);
        connection->SendLogMessage(worker::LogLevel::kInfo, "godot_core", entityAddedMsg);
    });
}

void Spatialos::processOps() {
    if (!isConnected) {
        return;
    }
    dispatcher->Process(connection->GetOpList(100));
}

void Spatialos::setPosition(std::int64_t entityId, double x, double y) {
    if (!isConnected) {
        return;
    }
    worker::EntityId oEntityId = entityId;
    improbable::Coordinates coordinates = improbable::Coordinates(x, 0, y);
    connection->SendComponentUpdate<improbable::Position>(
        oEntityId, improbable::Position::Update{}.set_coords(coordinates));
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
    connection->SendLogMessage(worker::LogLevel::kInfo, "godot_user", strConvert(msg));
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
