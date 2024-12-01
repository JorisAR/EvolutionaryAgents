#ifndef OPTIMIZABLE_PROBLEM_H
#define OPTIMIZABLE_PROBLEM_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot
{

class OptimizableProblem : public Node
{
    GDCLASS(OptimizableProblem, Node)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_parameter_count"), &OptimizableProblem::get_parameter_count);
        ClassDB::bind_method(D_METHOD("set_parameter_count", "count"), &OptimizableProblem::set_parameter_count);

        ADD_PROPERTY(PropertyInfo(Variant::INT, "parameter_count"), "set_parameter_count", "get_parameter_count");
        ADD_SIGNAL(MethodInfo("started", PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "action_vector")));
        ADD_SIGNAL(MethodInfo("ended", PropertyInfo(Variant::FLOAT, "fitness")));
    }

    int get_parameter_count() const
    {
        return parameter_count;
    }
    void set_parameter_count(int count)
    {
        parameter_count = std::max(1, count);
    }

  private:
    int parameter_count;
};

} // namespace godot

#endif // OPTIMIZABLE_PROBLEM_H
