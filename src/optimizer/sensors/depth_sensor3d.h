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
    static void _bind_methods();

    DepthSensor3D();
    ~DepthSensor3D();

    float get_is_binary() const;
    void set_is_binary(const float value);

    float get_max_distance() const;
    void set_max_distance(const float value);

    int get_collision_mask() const;
    void set_collision_mask(const int value);

    bool get_enabled() const;
    void set_enabled(const bool value);

    bool get_show_debug_ray() const;
    void set_show_debug_ray(const bool value);

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
