@tool
extends Node3D

@export var path: Path3D
@export var checkpoint_radius: float = 2.0
@export var checkpoint_frequency: float = 10.0
@export var checkpoint_scene: PackedScene  # Assign the Checkpoint.tscn scene here

@export var generate_checkpoints: bool:
	get:
		return false
	set(value):
		generate()

func generate() -> void:
	if path == null:
		printerr("Path is not defined.")
		return
	if checkpoint_scene == null:
		printerr("Checkpoint scene is not defined.")
		return

	# Remove existing checkpoints
	for child in get_children():
		child.owner = null;
		child.queue_free()

	var length = path.curve.get_baked_length()
	var current_distance = 0.0

	while current_distance <= length:
		var position = path.curve.sample_baked(current_distance)
		var checkpoint = checkpoint_scene.instantiate() as Node3D
		add_child(checkpoint)		
		checkpoint.owner = self.get_parent();
		
		checkpoint.name = "Checkpoint"
		checkpoint.global_transform.origin = position

		# Update the radius of the collider and mesh
		var staticbody = checkpoint.get_child(0) as StaticBody3D
		var collider = staticbody.get_child(1) as CollisionShape3D
		var shape = collider.shape as SphereShape3D
		shape.radius = checkpoint_radius

		var mesh_instance = staticbody.get_child(0) as MeshInstance3D
		var sphere_mesh = mesh_instance.mesh as SphereMesh
		sphere_mesh.radius = checkpoint_radius
		sphere_mesh.height = checkpoint_radius * 2
		current_distance += checkpoint_frequency

	print("Checkpoints generated along the path.")
