#ifndef OPTIMIZABLE_PROBLEM_H
#define OPTIMIZABLE_PROBLEM_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>

namespace godot
{

class OptimizableProblem : public Node
{
    GDCLASS(OptimizableProblem, Node)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_genome_size"), &OptimizableProblem::get_genome_size);
        ClassDB::bind_method(D_METHOD("set_genome_size", "count"), &OptimizableProblem::set_genome_size);

        ADD_PROPERTY(PropertyInfo(Variant::INT, "genome_size"), "set_genome_size", "get_genome_size");
        ADD_SIGNAL(MethodInfo("started", PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "genome")));
        ADD_SIGNAL(MethodInfo("ended", PropertyInfo(Variant::FLOAT, "fitness")));
        GDVIRTUAL_BIND(_evaluate, "genome");
    }

    int get_genome_size() const
    {
        return genome_size;
    }
    void set_genome_size(int count)
    {
        genome_size = std::max(1, count);
    }


    GDVIRTUAL1RC(float, _evaluate, PackedFloat32Array);
    virtual float _evaluate(const PackedFloat32Array &genome)
    {
        // Default implementation, should be overridden in GDScript
        UtilityFunctions::printerr("nope!.");
        return 0.0f;
    }

  private:
    int genome_size = 1;
};

} // namespace godot

#endif // OPTIMIZABLE_PROBLEM_H
