@tool
extends Node3D

@export var track : MeshInstance3D
@export var path: Path3D
@export var track_radius: float = 2.0
@export var path_sampling_frequency: float = 1.0
@export var wall_height: float = 2.0
@export var build_track: bool:
	get:
		return false
	set(value):
		rebuild()

func rebuild() -> void:
	if path == null:
		printerr("Path is not defined.")
		return
	if track == null:
		printerr("TrackMesh is not defined.")
		return

	var vertices = PackedVector3Array()
	var indices = PackedInt32Array()
	var uvs = PackedVector2Array()
	var normals = PackedVector3Array()
	var crosses = PackedVector3Array()

	var length = path.curve.get_baked_length()
	var current_distance = 0.0
	var vertex_index = 0

	while current_distance <= length:
		var position = path.curve.sample_baked(current_distance)
		var up_vector = path.curve.sample_baked_up_vector(current_distance)
		var tangent = (path.curve.sample_baked(current_distance + 0.001) - position).normalized()
		var cross = tangent.cross(up_vector).normalized()
		crosses.append(cross);
		
		var left_point = position - cross * track_radius
		var right_point = position + cross * track_radius

		# Add vertices for the track
		vertices.append(right_point)
		vertices.append(left_point)
		
		# Add UVs for the track
		var uv_x = current_distance / length
		uvs.append(Vector2(uv_x, 0))
		uvs.append(Vector2(uv_x, 1))

		# Add normals for the track
		normals.append(up_vector)
		normals.append(up_vector)

		if current_distance > 0:
			# Add indices for the track
			indices.append(vertex_index - 2) # previous right
			indices.append(vertex_index - 1) # previous left
			indices.append(vertex_index)     # current right

			indices.append(vertex_index - 1) # previous left
			indices.append(vertex_index + 1) # current left
			indices.append(vertex_index)     # current right

		current_distance += path_sampling_frequency
		vertex_index += 2

	# Close the loop for the track surface
	indices.append(vertex_index - 2) # previous right
	indices.append(vertex_index - 1) # previous left
	indices.append(0) # first right

	indices.append(vertex_index - 1) # previous left
	indices.append(1) # first left
	indices.append(0) # first right

	# Add walls
	var wall_base_index = vertex_index
	for i in range(0, vertices.size(), 2):
		var left_point = vertices[i + 1]
		var right_point = vertices[i]
		var left_top = left_point + Vector3(0, wall_height, 0)
		var right_top = right_point + Vector3(0, wall_height, 0)

		vertices.append(left_point)
		vertices.append(left_top)
		vertices.append(right_point)
		vertices.append(right_top)
		
		# UVs for the walls
		uvs.append(Vector2(0, 0))
		uvs.append(Vector2(0, 1))
		uvs.append(Vector2(1, 0))
		uvs.append(Vector2(1, 1))
		
		var cross = crosses[i / 2];
		# Normals for the walls
		var left_normal = Vector3(cross.x, 0, cross.z);
		var right_normal = Vector3(-cross.x, 0, -cross.z);
		normals.append(left_normal);
		normals.append(left_normal);
		normals.append(right_normal);
		normals.append(right_normal);
		
		
		if wall_base_index > vertex_index:
			# Left wall
			indices.append(wall_base_index - 4)
			indices.append(wall_base_index - 3)
			indices.append(wall_base_index + 0)

			indices.append(wall_base_index - 3)
			indices.append(wall_base_index + 1)
			indices.append(wall_base_index + 0)

			# Right wall
			indices.append(wall_base_index - 1)
			indices.append(wall_base_index - 2)		
			indices.append(wall_base_index + 2)

			indices.append(wall_base_index + 3)
			indices.append(wall_base_index - 1)
			indices.append(wall_base_index + 2)
		wall_base_index += 4
		
	indices.append(wall_base_index - 4)
	indices.append(wall_base_index - 3)
	indices.append(vertex_index + 0)

	indices.append(wall_base_index - 3)
	indices.append(vertex_index + 1)
	indices.append(vertex_index + 0)

	# Right wall
	indices.append(wall_base_index - 1)
	indices.append(wall_base_index - 2)		
	indices.append(vertex_index + 2)

	indices.append(vertex_index + 3)
	indices.append(wall_base_index - 1)
	indices.append(vertex_index + 2)

	# Create the mesh
	var mesh = ArrayMesh.new()
	var surface = Array()
	surface.resize(Mesh.ARRAY_MAX)
	surface[Mesh.ARRAY_VERTEX] = vertices
	surface[Mesh.ARRAY_INDEX] = indices
	surface[Mesh.ARRAY_TEX_UV] = uvs
	surface[Mesh.ARRAY_NORMAL] = normals
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, surface)
	
	# Assign the mesh to the MeshInstance
	track.mesh = mesh

	print("Track rebuilt with walls, UVs, and normals.")
	track.get_child(0).queue_free()
	track.create_trimesh_collision()
