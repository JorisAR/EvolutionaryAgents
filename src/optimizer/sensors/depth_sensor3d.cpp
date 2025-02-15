#include "depth_sensor3d.h"

void godot::DepthSensor3D::compute()
{
    Ref<World3D> world = get_world_3d();
    ERR_FAIL_COND(world.is_null());

    PhysicsDirectSpaceState3D *space_state =
        PhysicsServer3D::get_singleton()->space_get_direct_state(world->get_space());
    ERR_FAIL_NULL(space_state);

    Transform3D transform = get_global_transform();
    Vector3 origin = transform.origin;
    Vector3 direction = transform.basis.get_column(2);
    Ref<PhysicsRayQueryParameters3D> params =
        PhysicsRayQueryParameters3D::create(origin, origin + direction * max_distance, collision_mask);

    Dictionary raycast_result = space_state->intersect_ray(params);

    collided = raycast_result.size() > 0;
    if (collided)
    {
        output = is_binary ? 1.0f : origin.distance_to(raycast_result["position"]);
    }
    else
    {
        output = is_binary ? 0.0f : max_distance;
    }
}

void godot::DepthSensor3D::_notification(int p_what)
{
    switch (p_what)
    {
    case NOTIFICATION_ENTER_TREE: {

        set_physics_process_internal(true);

        _update_debug_shape();
    }
    break;

    case NOTIFICATION_EXIT_TREE: {

        set_physics_process_internal(false);

        _clear_debug_shape();
    }
    break;

    case NOTIFICATION_VISIBILITY_CHANGED: {
        if (is_inside_tree() && debug_instance.is_valid())
        {
            RenderingServer::get_singleton()->instance_set_visible(debug_instance, is_visible_in_tree());
        }
    }
    break;

    case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
        if (!enabled)
        {
            break;
        }
        if (is_inside_tree() && !godot::Engine::get_singleton()->is_editor_hint())
        {
            bool prev_collision_state = collided;
            compute();

            if (prev_collision_state != collided)
            {
                _update_debug_shape_material(true);
            }
        }

        if (is_inside_tree() && debug_instance.is_valid())
        {
            RenderingServer::get_singleton()->instance_set_transform(debug_instance, get_global_transform());
        }
    }
    break;
    }
}

void godot::DepthSensor3D::_update_debug_shape_vertices()
{
    debug_line_vertices.clear();

    if (std::abs(max_distance) < 0.01)
    {
        return;
    }

    debug_line_vertices.push_back(Vector3());
    debug_line_vertices.push_back(Vector3(0, 0, max_distance));
}

void godot::DepthSensor3D::_create_debug_shape()
{
    _update_debug_shape_material(false);

    if (!debug_instance.is_valid())
    {
        debug_instance = godot::RenderingServer::get_singleton()->instance_create();
    }

    if (debug_mesh.is_null())
    {
        debug_mesh.instantiate();
    }
}

void godot::DepthSensor3D::_update_debug_shape_material(bool p_check_collision)
{
    if (!debug_material.is_valid())
    {
        Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
        debug_material = material;

        material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
        material->set_flag(StandardMaterial3D::FLAG_DISABLE_FOG, true);
        // Use double-sided rendering so that the RayCast can be seen if the camera is inside.
        material->set_cull_mode(BaseMaterial3D::CULL_DISABLED);
        material->set_transparency(BaseMaterial3D::TRANSPARENCY_ALPHA);
    }

    // Color color = debug_shape_custom_color;
    Color color = Color(1.0, 0, 0, 1.0f);
    if (p_check_collision && collided)
    {
        if ((color.get_h() < 0.055 || color.get_h() > 0.945) && color.get_s() > 0.5 && color.get_v() > 0.5)
        {
            // If base color is already quite reddish, highlight collision with green color
            color = Color(0.0, 1.0, 0.0, color.a);
        }
        else
        {
            // Else, highlight collision with red color
            color = Color(1.0, 0, 0, color.a);
        }
    }

    Ref<StandardMaterial3D> material = static_cast<Ref<StandardMaterial3D>>(debug_material);
    material->set_albedo(color);
}

void godot::DepthSensor3D::_update_debug_shape()
{
    if (!enabled || !show_debug_ray)
    {
        _clear_debug_shape();
        return;
    }

    if (!debug_instance.is_valid())
    {
        _create_debug_shape();
    }

    if (!debug_instance.is_valid() || debug_mesh.is_null())
    {
        return;
    }

    _update_debug_shape_vertices();

    debug_mesh->clear_surfaces();

    Array a;
    a.resize(Mesh::ARRAY_MAX);

    uint32_t flags = 0;
    int surface_count = 0;

    if (!debug_line_vertices.is_empty())
    {
        a[Mesh::ARRAY_VERTEX] = debug_line_vertices;
        debug_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_LINES, a, Array(), Dictionary(), flags);
        debug_mesh->surface_set_material(surface_count, debug_material);
        ++surface_count;
    }

    RenderingServer::get_singleton()->instance_set_base(debug_instance, debug_mesh->get_rid());
    if (is_inside_tree())
    {
        RenderingServer::get_singleton()->instance_set_scenario(debug_instance, get_world_3d()->get_scenario());
        RenderingServer::get_singleton()->instance_set_visible(debug_instance, is_visible_in_tree());
        RenderingServer::get_singleton()->instance_set_transform(debug_instance, get_global_transform());
    }
}

void godot::DepthSensor3D::_clear_debug_shape()
{
    ERR_FAIL_NULL(RenderingServer::get_singleton());
    if (debug_instance.is_valid())
    {
        RenderingServer::get_singleton()->free_rid(debug_instance);
        debug_instance = RID();
    }
    if (debug_mesh.is_valid())
    {
        RenderingServer::get_singleton()->free_rid(debug_mesh->get_rid());
        debug_mesh = Ref<ArrayMesh>();
    }
}

void godot::DepthSensor3D::_bind_methods()
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

godot::DepthSensor3D::DepthSensor3D()
{
}

godot::DepthSensor3D::~DepthSensor3D()
{
}

float godot::DepthSensor3D::get_is_binary() const
{
    return is_binary;
}

void godot::DepthSensor3D::set_is_binary(const float value)
{
    is_binary = value;
}

float godot::DepthSensor3D::get_max_distance() const
{
    return max_distance;
}

void godot::DepthSensor3D::set_max_distance(const float value)
{
    max_distance = value;
    _update_debug_shape();
}

int godot::DepthSensor3D::get_collision_mask() const
{
    return collision_mask;
}

void godot::DepthSensor3D::set_collision_mask(const int value)
{
    collision_mask = value;
}

bool godot::DepthSensor3D::get_enabled() const
{
    return enabled;
}

void godot::DepthSensor3D::set_enabled(const bool value)
{
    enabled = value;
    collided = false;
    output = is_binary ? 0.0f : max_distance;
    _update_debug_shape();
}

bool godot::DepthSensor3D::get_show_debug_ray() const
{
    return show_debug_ray;
}

void godot::DepthSensor3D::set_show_debug_ray(const bool value)
{
    show_debug_ray = value;
    _update_debug_shape();
}