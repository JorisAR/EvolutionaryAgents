extends CharacterBody3D

@export var agent: Agent;
@export var speed:= 5;
@export var rotation_speed:= 1;

@export var sensors: Array[DepthSensor3D];
var active: bool = false;

func get_agent() -> Agent:
	return agent;

var _pos : Vector3;
var _rot : Vector3;
var time = 0;

func _ready() -> void:
	_pos = global_position;
	_rot = global_rotation;
	
	if(sensors.size() != 5):
		printerr("Should be 5 depth sensors")

func _physics_process(delta: float) -> void:
	if(!active):
		return;
	
	var pos = global_position;
	var vel = velocity;
	
	var state = [pos.x,pos.y,pos.z,vel.x,vel.y,vel.z];
	state.append_array([
		sensors[0].get_output(),
		sensors[1].get_output(),
		sensors[2].get_output(),
		sensors[3].get_output(),
		sensors[4].get_output(),
	]);
	var action = Array(agent.infer(PackedFloat32Array(state)));
		
	#match Agent.get_max_element_index(action):
	match Agent.sample_weighted_index(Agent.soft_max(action), randf()):
		1:
			rotate_y(-rotation_speed * delta)
		2:
			rotate_y(rotation_speed * delta)	
	
	var forward = -transform.basis.z.normalized();
	velocity = forward * speed;
	move_and_slide();
	
	if (time > 300):
		_on_round_end();
	
	time += delta;

func _on_round_end() -> void:
	active = false;
	#we only have one agent per room, so we can emit the rooms signal from here:
	agent.ended.emit();	

func _on_agent_started() -> void:
	time = 0;
	agent.fitness = 0.0;
	active = true;
	global_position = _pos;
	global_rotation = _rot;

func _on_area_3d_area_entered(area: Area3D) -> void:
	agent.fitness += 1;


func _on_area_3d_body_entered(body: Node3D) -> void:
	agent.fitness -= 5;
	_on_round_end();
