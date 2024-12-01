extends ProblemOptimizer

func _ready() -> void:
	start_training.call_deferred();

func _exit_tree() -> void:
	end_training();
	
func _input(event):
	if event is InputEventKey and event.is_pressed() and event.physical_keycode == KEY_ENTER:
		end_training();
