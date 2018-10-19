/* spatialos.cpp */

#include "spatialos.h"
#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>

const std::string kWorkerType = "GodotServer";
const std::string kReceptionistIp = "localhost";
const std::uint16_t kReceptionistPort = 7777;

using ComponentRegistry = worker::Components<improbable::Position, improbable::Metadata>;

worker::Connection ConnectWithReceptionist(const std::string hostname,
                                           const std::uint16_t port,
                                           const std::string& worker_id,
                                           const worker::ConnectionParameters& connection_parameters) {
    auto future = worker::Connection::ConnectAsync(ComponentRegistry{}, hostname, port, worker_id, connection_parameters);
    return future.Get();
}

void Spatialos::joinGame() {
    std::cout << "Connecting worker" << std::endl;

    worker::ConnectionParameters parameters;
    parameters.WorkerType = kWorkerType;
    parameters.Network.ConnectionType = worker::NetworkConnectionType::kTcp;
    parameters.Network.UseExternalIp = false;

    std::string id = kWorkerType;
    id += std::to_string(workerId);

    connection.reset(new worker::Connection{ConnectWithReceptionist(kReceptionistIp, kReceptionistPort, id, parameters)});

    connection->SendLogMessage(worker::LogLevel::kInfo, "godot_logger", "Hello from Godot!");
    
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
        connection->SendLogMessage(worker::LogLevel::kInfo, "godot_logger", entityAddedMsg);
    });
}

void Spatialos::processOps() {
    if (!isConnected) {
        return;
    }
    dispatcher->Process(connection->GetOpList(100));
}

void Spatialos::setPosition(double x, double y) {
    if (!isConnected) {
        return;
    }
    worker::EntityId entityId = 1;
    improbable::Coordinates coordinates = improbable::Coordinates(x, 0, y);
    //improbable::PositionData positionData = improbable::PositionData(coordinates);
    connection->SendComponentUpdate<improbable::Position>(
        entityId, improbable::Position::Update{}.set_coords(coordinates));
}

void Spatialos::_bind_methods() {
    ClassDB::bind_method(D_METHOD("join_game"), &Spatialos::joinGame);
    ClassDB::bind_method(D_METHOD("process_ops"), &Spatialos::processOps);
    ClassDB::bind_method(D_METHOD("set_position", "x", "y"), &Spatialos::setPosition);
}

Spatialos::Spatialos() {
    workerId = 0;
    isConnected = false;
}

