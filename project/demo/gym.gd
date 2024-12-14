extends Gym

var _init_scale = 1000;

func _ready() -> void:
	_init_scale = time_scale;
	agents = []
	for c in get_children():
		if c.has_method("get_agent"):
			agents.push_back(c.get_agent());
	print(agents.size(), " agents found")
	set_agents(agents);
	toggle_time_scale();

var is_fast_time = false;
func toggle_time_scale():
	set_time_scale(1 if is_fast_time else _init_scale);
	is_fast_time = !is_fast_time;
	
func _input(event): 
	if event is InputEventKey and event.is_pressed() and event.physical_keycode == KEY_SPACE:
		toggle_time_scale();
	if event is InputEventKey and event.is_pressed() and event.physical_keycode == KEY_ENTER:
		end_training();
