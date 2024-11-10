extends EvolutionaryGym

@export var time_scale := 1;

func _ready() -> void:	
	rooms = []
	for c in get_children():
		if c.has_method("get_room"):
			rooms.push_back(c.get_room());
	set_rooms(rooms);
	toggle_time_scale();
	start_training();

var is_fast_time = false;
func toggle_time_scale():
	set_time_scale(1 if is_fast_time else time_scale);
	is_fast_time = !is_fast_time;

func set_time_scale(scale: int):
	Engine.max_fps = 30;
	Engine.time_scale = scale;
	Engine.physics_ticks_per_second = 20 * scale;
	Engine.max_physics_steps_per_frame = max(1,ceili(float(Engine.physics_ticks_per_second) / Engine.max_fps));

func _exit_tree() -> void:
	end_training();
	
func _input(event): 
	if event is InputEventKey and event.is_pressed() and event.physical_keycode == KEY_SPACE:
		toggle_time_scale();
	if event is InputEventKey and event.is_pressed() and event.physical_keycode == KEY_ENTER:
		end_training();
