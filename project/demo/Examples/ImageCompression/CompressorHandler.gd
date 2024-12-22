extends OptimizableProblem

@export var dispatcher: SplatRenderer;

var wait = false;

func _enter_tree() -> void:
	genome_size = dispatcher.SPLAT_FLOAT_COUNT * dispatcher.SPLAT_COUNT;

func _evaluate(genome: PackedFloat32Array) -> float:
	if(wait):
		await get_tree().create_timer(0.0001).timeout
	wait = false
	return dispatcher.compute_fitness(genome.to_byte_array());


func _on_started(genome: PackedFloat32Array) -> void:
	if(wait):
		await get_tree().create_timer(0.0001).timeout
	wait = false
	#self.ended.emit(Time.get_ticks_msec());
	self.ended.emit(dispatcher.compute_fitness(genome.to_byte_array()));


func _on_image_compression_generation_ended() -> void:
	wait = true;
