extends CharacterBody3D

@export var auto_start := false
@export var agent: EvolutionaryAgent;
@export var speed:= 5;
@export var rotation_speed:= 1;

@export var sensors: Array[DepthSensor3D];
var active: bool = false;

func get_agent() -> EvolutionaryAgent:
	return agent;

var _pos : Vector3;
var _rot : Vector3;
var time = 0;

func _ready() -> void:
	_pos = global_position;
	_rot = global_rotation;
	
	if(sensors.size() != 5):
		printerr("Should have exactly 5 depth sensors")
	
	if(auto_start):
		agent.start()

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
	
	#NOTE:
	#By using sample_weighted_index based on soft_max, we treat the output of the network as a
	#discrete probaility mass function over the possible moves. We sample it, and steer if its 1 or 2.
	#no action is performed if we get 0.
	match EvolutionaryAgent.sample_weighted_index(EvolutionaryAgent.soft_max(action), randf()):
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
	agent.ended.emit();	

#called by the agents start signal
func _on_agent_started() -> void:
	time = 0;
	agent.fitness = 0.0;
	active = true;
	global_position = _pos;
	global_rotation = _rot;

#called when colliding with a reward checkpoint
func _on_area_3d_area_entered(area: Area3D) -> void:
	agent.fitness += 1; #arbitrarily chosen positive reward for progressing in the level

#called when colliding with a wall
func _on_area_3d_body_entered(body: Node3D) -> void:
	agent.fitness -= 5; #arbitrarily chosen negative reward for crashing
	_on_round_end();
