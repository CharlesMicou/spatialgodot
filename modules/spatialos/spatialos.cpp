/* spatialos.cpp */

#include "spatialos.h"
#include <improbable/worker.h>
#include <iostream>

const std::string kWorkerType = "godot";
const std::string kReceptionistIp = "localhost";
const std::uint16_t kReceptionistPort = 7777;

using ComponentRegistry = worker::Components<>;

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

    worker::Connection connection = ConnectWithReceptionist(kReceptionistIp, kReceptionistPort, id, parameters);

    connection.SendLogMessage(worker::LogLevel::kInfo, "godot_logger", "Hello from Godot!");
    
    worker::Dispatcher dispatcher{ComponentRegistry{}};

    bool isConnected = true;
    dispatcher.OnDisconnect([&](const worker::DisconnectOp& op) {
        std::cerr << "[disconnect] " << op.Reason << std::endl;
        isConnected = false;
    });

    while (isConnected) {
        dispatcher.Process(connection.GetOpList(100));
    }
}

void Spatialos::_bind_methods() {
    ClassDB::bind_method(D_METHOD("join_game"), &Spatialos::joinGame);
}

Spatialos::Spatialos() {
    workerId = 0;
}

