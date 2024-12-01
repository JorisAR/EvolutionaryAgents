extends OptimizableProblem

enum ProblemType {
	ROSENBROCK,
	POLYNOMIAL,
	LEAST_SQUARES
}

@export var problem_type = ProblemType.ROSENBROCK

var coeff = []
var solution_vector = PackedFloat32Array()

func _init():
	for i in range(genome_size):
		coeff.append(randf_range(-10, 10))
	solution_vector.resize(genome_size)
	for i in range(genome_size):
		solution_vector[i] = randf_range(-10, 10)
		
	print(solution_vector)

# Generalized Rosenbrock function
func rosenbrock(genome: PackedFloat32Array) -> float:
	var a = coeff[0]
	var b = coeff[1]
	var sum = 0.0
	for i in range(genome.size() - 1):
		var xi = genome[i]
		var xi1 = genome[i + 1]
		sum += pow((a - xi), 2) + b * pow((xi1 - xi * xi), 2)
	return sum

# Generalized Polynomial function
func polynomial(genome: PackedFloat32Array) -> float:
	var sum = 0.0
	for i in range(genome.size()):
		var xi = genome[i]
		sum += coeff[i % coeff.size()] * pow(xi, 4) - 3 * pow(xi, 3) + 2 * pow(xi, 2)
	return sum

# Least Squares function
func least_squares(genome: PackedFloat32Array) -> float:
	var sum = 0.0
	for i in range(genome.size()):
		var diff = genome[i] - solution_vector[i]
		sum += diff * diff
	return sum

func _on_started(genome: PackedFloat32Array) -> void:
	var fitness = 0.0
	if problem_type == ProblemType.ROSENBROCK:
		fitness = -rosenbrock(genome)
	elif problem_type == ProblemType.POLYNOMIAL:
		fitness = -polynomial(genome)
	else:
		fitness = -least_squares(genome)

	self.ended.emit(fitness)
