extends Node3D

@export var agent: Agent;
@export var character: CharacterBody3D;
@export var active: bool;
@export var movement_speed = 5;

func get_agent() -> Agent:
	return agent;
	
var _character_pos : Vector3;
var time = 0;

func _ready() -> void:
	_character_pos = character.global_position;
	
func _move_character(velocity : Vector3) -> void:
	character.velocity = velocity;
	character.move_and_slide();
	pass
		
func _physics_process(delta: float) -> void:
	if(!active):
		return;
	
	var characterPos = character.position;	
	if(abs(characterPos.x) > 10 or abs(characterPos.z) > 10):
		agent.fitness = -1;
		_on_round_end();
		
	var state = [characterPos.x, characterPos.y, characterPos.z]
	var action = Array(agent.infer(PackedFloat32Array(state)));
		
	var dir = Vector3.ZERO;
	
	match Agent.get_max_element_index(action):
		1:
			dir = Vector3(1, 0, 0);
		2:
			dir = Vector3(-1, 0, 0);
		3:
			dir = Vector3(0, 0, 1);
		4:
			dir = Vector3(0, 0, -1);
			
	_move_character(movement_speed * dir);

	agent.fitness -= delta * 0.1;
	if (time > 60):
		_on_round_end();
	time += delta;
	
func _on_round_end() -> void:
	active = false;
	agent.ended.emit();
	
	
func _on_big_body_entered(body: Node3D) -> void:
	agent.fitness += 50;
	_on_round_end();

func _on_small_body_entered(body: Node3D) -> void:
	agent.fitness += 5;
	_on_round_end();


func _on_agent_started() -> void:
	time = 0;
	agent.fitness = 0;
	active = true;
	character.global_position = _character_pos;
	character.global_position += Vector3(randf() * 6 - 3, 0, randf() * 6 - 3);
