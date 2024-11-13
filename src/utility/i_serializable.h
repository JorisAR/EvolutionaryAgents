#ifndef I_SERIALIZABLE_H
#define I_SERIALIZABLE_H

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/json.hpp>

template <typename T>
class ISerializable
{
public:
    virtual ~ISerializable() {}
    virtual godot::Dictionary to_dict() const = 0;
    static T* from_dict(const godot::Dictionary &dict)
    {
        T* instance = new T();
        instance->deserialize_from_dict(dict);
        return instance;
    }
protected:
    virtual void deserialize_from_dict(const godot::Dictionary &dict) = 0;
};

#endif // I_SERIALIZABLE_H
