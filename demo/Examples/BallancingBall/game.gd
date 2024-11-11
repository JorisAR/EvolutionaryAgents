extends Node3D

@export var agent: Agent;
@export var ball: RigidBody3D;
@export var platform: CharacterBody3D;
@export var active: bool;


func get_agent() -> Agent:
	return agent;

var _ball_pos : Vector3;
var _platform_rot : Vector3;
var time = 0;

func _ready() -> void:
	_ball_pos = ball.global_position;
	_platform_rot = platform.global_rotation;
	
func _physics_process(delta: float) -> void:
	if(!active):
		return;
	
	var platformPos = platform.global_position;	
	var ballPos = ball.global_position - platformPos;
	var ballVel = ball.linear_velocity;
	var platformRot = platform.global_rotation;
	
	var state = [ballPos.x, ballPos.y, ballPos.z, ballVel.x, ballVel.y, ballVel.z, platformRot.x, platformRot.z]	 
	var action = Array(agent.infer(PackedFloat32Array(state)));	
	platform.global_rotation = Vector3(action[0], 0, action[1]);
	
	var dist = Vector2(ballPos.x, ballPos.z).distance_to(Vector2(0, 0));
	agent.fitness += delta / (dist + 0.01);
	
	if (ballPos.y < -2 || dist > 2.9):
		agent.fitness -= 1.0;
		_on_round_end();
		
	if (time > 60):
		_on_round_end();
	
	time += delta;

func _on_round_end() -> void:
	active = false;	
	ball.get_child(1).disabled = true;
	#print(agent.fitness);
	
	#we only have one agent per room, so we can emit the rooms signal from here:
	agent.ended.emit();
	

func _on_agent_started() -> void:
	time = 0;
	agent.fitness = 0.1;
	active = true;
	ball.global_position = _ball_pos;
	platform.global_rotation = _platform_rot;
	#ball.linear_velocity = 1.5 * Vector3(1,0,0).normalized();
	var angle = randf() * 2 * PI;
	ball.linear_velocity = 1.5 * Vector3(cos(angle), 0, sin(angle)).normalized();
	ball.get_child(1).disabled = false;
