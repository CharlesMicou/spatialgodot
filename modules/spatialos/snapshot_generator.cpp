#include "snapshot_generator.h"
#include <godotcore/godot_position2d.h>
#include <improbable/standard_library.h>
#include <godotcore/auto_instantiable.h>
#include "improbable/worker.h"
#include "spatial_util.h"
#include "component_registry.h"
#include <iostream>

worker::Entity make_ball(float x, float y) {
    worker::Entity builder;

    godotcore::GodotCoordinates2D gpos({{0, 0}, {x, y}});
    
    builder.Add<improbable::Metadata>({"TestBall"});
    builder.Add<improbable::Persistence>({});
    builder.Add<godotcore::GodotPosition2D>({gpos, {}});
    builder.Add<godotcore::AutoInstantiable>({"res://auto_scene/TestBall.tscn"});
    builder.Add<improbable::Position>({fromGodotPosition(gpos)});

    // ACL must be done last if we want it to pick up components automatically
    builder.Add<improbable::EntityAcl>({clientAndServerReqSet, make_component_acl(builder)});
    
    return builder;
}

worker::Entity make_rock(float x, float y) {
    worker::Entity builder;

    godotcore::GodotCoordinates2D gpos({{0, 0}, {x, y}});
    
    builder.Add<improbable::Metadata>({"Rock"});
    builder.Add<improbable::Persistence>({});
    builder.Add<godotcore::GodotPosition2D>({gpos, {}});
    builder.Add<godotcore::AutoInstantiable>({"res://auto_scene/rock1.tscn"});
    builder.Add<improbable::Position>({fromGodotPosition(gpos)});

    // ACL must be done last if we want it to pick up components automatically
    builder.Add<improbable::EntityAcl>({clientAndServerReqSet, make_component_acl(builder)});
    
    return builder;
}

worker::Entity make_tree(float x, float y) {
    worker::Entity builder;

    godotcore::GodotCoordinates2D gpos({{0, 0}, {x, y}});
    
    builder.Add<improbable::Metadata>({"Tree"});
    builder.Add<improbable::Persistence>({});
    builder.Add<godotcore::GodotPosition2D>({gpos, {}});
    builder.Add<godotcore::AutoInstantiable>({"res://auto_scene/tree1.tscn"});
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
    ostream.WriteEntity(6, make_rock(-200, -200));
    ostream.WriteEntity(7, make_rock(200, 200));
    ostream.WriteEntity(8, make_tree(300, 350));
    ostream.WriteEntity(9, make_tree(-300, 0));
    ostream.WriteEntity(10, make_tree(0, 500));
    std::cout << "Finished writing snapshot" << std::endl;
}

void SnapshotGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("take_snapshot", "absolute_path_to_snapshot"), &SnapshotGenerator::takeSnapshot);
}

SnapshotGenerator::SnapshotGenerator() {
}
