#ifndef SENSOR3D_H
#define SENSOR3D_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot
{

class Sensor3D : public Node3D
{
    GDCLASS(Sensor3D, Node3D)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_output"), &Sensor3D::get_output);
    }

    Sensor3D()
    {
    }
    ~Sensor3D()
    {
    }

    virtual float get_output() const {
      return output;
    }

  protected:
    virtual void compute() {};
    float output = 0;
};

} // namespace godot

#endif // SENSOR_H
