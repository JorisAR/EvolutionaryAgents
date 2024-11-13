#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "i_serializable.h"
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot
{

class JSONUtils
{
  public:
    template <typename T> static godot::String serialize(const ISerializable<T> &obj)
    {
        godot::Dictionary dict = obj.to_dict();
        return godot::JSON::stringify(dict, "\t", true, true);
    }

    template <typename T> static T *deserialize(const godot::String &data)
    {
        godot::Variant parsed_result = godot::JSON::parse_string(data);
        if (parsed_result.get_type() == godot::Variant::DICTIONARY)
        {
            return ISerializable<T>::from_dict(parsed_result.operator godot::Dictionary());
        }
        return nullptr;
    }

    template <typename T> static void save_to_file(const ISerializable<T> &obj, const godot::String &file_path)
    {
        godot::Ref<godot::FileAccess> file = godot::FileAccess::open(file_path, godot::FileAccess::WRITE);
        if (file.is_valid())
        {
            godot::String data = serialize(obj);
            file->store_string(data);
            file->close();
            return;
        }

        UtilityFunctions::printerr(static_cast<String>("Error saving file: " + file_path));
    }

    template <typename T> static T *load_from_file(const godot::String &file_path)
    {
        godot::Ref<godot::FileAccess> file = godot::FileAccess::open(file_path, godot::FileAccess::READ);
        if (file.is_valid())
        {
            godot::String data = file->get_as_text();
            file->close();
            return deserialize<T>(data);
        }

        UtilityFunctions::printerr(static_cast<String>("Error loading file: " + file_path));

        return nullptr;
    }
};
} // namespace godot
#endif // JSON_UTILS_H
