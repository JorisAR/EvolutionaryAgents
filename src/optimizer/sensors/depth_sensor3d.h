#ifndef DEPTH_SENSOR3D_H
#define DEPTH_SENSOR3D_H

#include "sensor3d.h"
#include <godot_cpp/classes/World3D.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters3d.hpp>
#include <godot_cpp/classes/physics_server3d.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot
{

class DepthSensor3D : public Sensor3D
{
    GDCLASS(DepthSensor3D, Sensor3D)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("set_is_binary", "value"), &DepthSensor3D::set_is_binary);
        ClassDB::bind_method(D_METHOD("get_is_binary"), &DepthSensor3D::get_is_binary);
        ClassDB::bind_method(D_METHOD("set_max_distance", "value"), &DepthSensor3D::set_max_distance);
        ClassDB::bind_method(D_METHOD("get_max_distance"), &DepthSensor3D::get_max_distance);
        ClassDB::bind_method(D_METHOD("set_collision_mask", "value"), &DepthSensor3D::set_collision_mask);
        ClassDB::bind_method(D_METHOD("get_collision_mask"), &DepthSensor3D::get_collision_mask);

        ClassDB::bind_method(D_METHOD("set_show_debug_ray", "value"), &DepthSensor3D::set_show_debug_ray);
        ClassDB::bind_method(D_METHOD("get_show_debug_ray"), &DepthSensor3D::get_show_debug_ray);

        ClassDB::bind_method(D_METHOD("set_enabled", "value"), &DepthSensor3D::set_enabled);
        ClassDB::bind_method(D_METHOD("get_enabled"), &DepthSensor3D::get_enabled);

        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_enabled"), "set_enabled", "get_enabled");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_binary"), "set_is_binary", "get_is_binary");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_distance"), "set_max_distance", "get_max_distance");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_3D_PHYSICS),
                     "set_collision_mask", "get_collision_mask");

        ADD_GROUP("Debug", "debug");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_show_debug_ray"), "set_show_debug_ray", "get_show_debug_ray");
    }

    DepthSensor3D()
    {
    }
    ~DepthSensor3D()
    {
    }

    float get_is_binary() const
    {
        return is_binary;
    }
    void set_is_binary(const float value)
    {
        is_binary = value;
    }

    float get_max_distance() const
    {
        return max_distance;
    }
    void set_max_distance(const float value)
    {
        max_distance = value;

        _update_debug_shape();
    }

    int get_collision_mask() const
    {
        return collision_mask;
    }
    void set_collision_mask(const int value)
    {
        collision_mask = value;
    }

    bool get_enabled() const
    {
        return enabled;
    }
    void set_enabled(const bool value)
    {
        enabled = value;
        collided = false;
        output = is_binary ? 0.0f : max_distance;
        _update_debug_shape();
    }

    bool get_show_debug_ray() const
    {
        return show_debug_ray;
    }
    void set_show_debug_ray(const bool value)
    {
        show_debug_ray = value;

        _update_debug_shape();
    }

  protected:
    void compute() override;

    void _notification(int p_what);

  private:
    void _update_debug_shape_vertices();

    const std::vector<Vector3> &get_debug_line_vertices() const;

    void _create_debug_shape();

    void _update_debug_shape_material(bool p_check_collision);

    void _update_debug_shape();

    void _clear_debug_shape();
    bool is_binary = false;
    float max_distance = 10.0f;
    uint32_t collision_mask = 1;

    bool enabled = true;
    bool collided = false;
    bool show_debug_ray = true;
    PackedVector3Array debug_line_vertices;
    Ref<ArrayMesh> debug_mesh;
    Ref<StandardMaterial3D> debug_material;
    RID debug_instance;
};

} // namespace godot

#endif // DEPTH_SENSOR3D_H
