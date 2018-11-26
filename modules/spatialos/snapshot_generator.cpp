#include "snapshot_generator.h"
#include <godotcore/godot_position2d.h>
#include <improbable/standard_library.h>
#include "improbable/worker.h"
#include "spatial_util.h"
#include "component_registry.h"
#include <iostream>

worker::Map<worker::ComponentId, improbable::WorkerRequirementSet> make_component_acl(const worker::Entity& entity) {
    worker::Map<worker::ComponentId, improbable::WorkerRequirementSet> builder;
    auto it = entity.GetComponentIds().begin();
    while (it != entity.GetComponentIds().end()) {
        auto should_simulate = simulatedComponents.find(*it);
        if (should_simulate != simulatedComponents.end()) {
            builder.insert({{*it, serverReqSet}});
        }
        it++;
    }
    return builder;
}

worker::Entity make_ball(float x, float y) {
    worker::Entity builder;

    godotcore::GodotPosition2DData gpos({{0, 0}, {x, y}, {0, 0}});
    
    builder.Add<improbable::Metadata>({"TestBall"});
    builder.Add<improbable::Persistence>({});
    builder.Add<godotcore::GodotPosition2D>(gpos);
    builder.Add<improbable::Position>({fromGodotPosition(gpos)});

    // ACL must be done last if we want it to pick up components automatically
    builder.Add<improbable::EntityAcl>({clientAndServerReqSet, make_component_acl(builder)});
    
    return builder;
}

void SnapshotGenerator::takeSnapshot(String path) {
    std::string p = fromGodotString(path);
    worker::SnapshotOutputStream ostream{MergedComponentRegistry(), p};
    std::cout << "Writing snapshot to " << p << std::endl;

    ostream.WriteEntity(1, make_ball(-100, -100));
    ostream.WriteEntity(2, make_ball(100, 100));
    ostream.WriteEntity(3, make_ball(-100, 100));
    ostream.WriteEntity(4, make_ball(100, -100));
    ostream.WriteEntity(5, make_ball(150, -155));
    std::cout << "Finished writing snapshot" << std::endl;
}

void SnapshotGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("take_snapshot", "absolute_path_to_snapshot"), &SnapshotGenerator::takeSnapshot);
}

SnapshotGenerator::SnapshotGenerator() {
}
