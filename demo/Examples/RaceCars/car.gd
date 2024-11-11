extends RigidBody3D

@export var agent: Agent;
var active: bool = false;

func get_agent() -> Agent:
	return agent;

var _pos : Vector3;
var time = 0;

func _ready() -> void:
	_pos = global_position;
	
func _physics_process(delta: float) -> void:
	if(!active):
		return;
	
	var pos = global_position;
	var vel = linear_velocity;
	
	var state = [pos.x,pos.y,pos.z,vel.x,vel.y,vel.z];
	var action = Array(agent.infer(PackedFloat32Array(state)));
		
	var rotate = 0;
	
	match Agent.get_max_element_index(action):
		1:
			rotate = -1;
		2:
			rotate = 1;
	
	
		
	if (time > 120):
		_on_round_end();
	
	time += delta;

func _on_round_end() -> void:
	active = false;
	#we only have one agent per room, so we can emit the rooms signal from here:
	agent.ended.emit();
	

func _on_agent_started() -> void:
	time = 0;
	agent.fitness = 0.1;
	active = true;
	
	global_position = _pos;


func _on_body_entered(body: Node) -> void:
	agent.fitness -= 5;
