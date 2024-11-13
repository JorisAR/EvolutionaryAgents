#ifndef UTILS_H
#define UTILS_H

#include <godot_cpp/classes/object.hpp>
#include <vector>

namespace godot {

class Utils {
public:
    Utils() { }
    ~Utils() { }

static PackedFloat32Array vector_to_array_float(const std::vector<float>& vec) {
    PackedFloat32Array array;
    array.resize(vec.size());
    std::copy(vec.begin(), vec.end(), array.ptrw());
    return array;
}

static std::vector<float> array_to_vector_float(const PackedFloat32Array& array) {
    std::vector<float> vec(array.size());
    std::copy(array.ptr(), array.ptr() + array.size(), vec.begin());
    return vec;
}

static PackedInt32Array vector_to_array_int(const std::vector<int>& vec) {
    PackedInt32Array array;
    array.resize(vec.size());
    std::copy(vec.begin(), vec.end(), array.ptrw());
    return array;
}

static std::vector<int> array_to_vector_int(const PackedInt32Array& array) {
    std::vector<int> vec(array.size());
    std::copy(array.ptr(), array.ptr() + array.size(), vec.begin());
    return vec;
}

};

}

#endif
