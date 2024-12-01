extends OptimizableProblem

@export var dispatcher: SplatRenderer;

var wait = false;

func _on_started(genome: PackedFloat32Array) -> void:
	if(wait):
		await get_tree().create_timer(0.001).timeout
	wait = false
	self.ended.emit(-dispatcher.compute_fitness(genome.to_byte_array()));


func _on_image_compression_generation_ended() -> void:
	wait = true;
