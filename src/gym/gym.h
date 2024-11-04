#ifndef GYM_H
#define GYM_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

	class Gym : public Node {
		GDCLASS(Gym, Node)

	protected:
		static void _bind_methods();

	public:
		Gym();
		~Gym();
		void _process(double delta) override;
	};
}

#endif