#include "spatialos.h"
#include "core/ustring.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>

using ComponentRegistry = worker::Components<improbable::Position, improbable::Metadata>;

worker::Connection ConnectWithReceptionist(const std::string hostname,
                                           const std::uint16_t port,
                                           const std::string& worker_id,
                                           const worker::ConnectionParameters& connection_parameters) {
    auto future = worker::Connection::ConnectAsync(ComponentRegistry{}, hostname, port, worker_id, connection_parameters);
    return future.Get();
}

std::string strConvert(const String &godotString) {
    std::wstring wide(godotString.ptr());
    std::string s(wide.begin(), wide.end());
    return s;
}

void Spatialos::joinGame(const String &receptionistIp, const int receptionistPort, const String &id, const String &type) {
    workerId = id;
    workerType = type;
    worker::ConnectionParameters parameters;
    parameters.WorkerType = strConvert(workerType);
    parameters.Network.ConnectionType = worker::NetworkConnectionType::kTcp;
    parameters.Network.UseExternalIp = false;

    connection.reset(new worker::Connection{ConnectWithReceptionist(strConvert(receptionistIp), receptionistPort, strConvert(workerId), parameters)});

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

void Spatialos::sendInfoMessage(const String &msg) {
    if (!isConnected) {
        return;
    }
    connection->SendLogMessage(worker::LogLevel::kInfo, "godot_user", strConvert(msg));
}

void Spatialos::_bind_methods() {
    ClassDB::bind_method(D_METHOD("join_game", "receptionist_host", "receptionist_port", "worker_id", "worker_type"), &Spatialos::joinGame);
    ClassDB::bind_method(D_METHOD("process_ops"), &Spatialos::processOps);
    ClassDB::bind_method(D_METHOD("set_position", "entityId", "x", "y"), &Spatialos::setPosition);
    ClassDB::bind_method(D_METHOD("send_log", "msg"), &Spatialos::sendInfoMessage);
}

Spatialos::Spatialos() {
    workerId = "defaultworkerid";
    workerType = "defaultworkertype";
    isConnected = false;
}
