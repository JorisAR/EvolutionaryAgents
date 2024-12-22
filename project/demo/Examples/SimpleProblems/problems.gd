extends OptimizableProblem

enum ProblemType {
	ROSENBROCK,
	POLYNOMIAL,
	LEAST_SQUARES,
	SUM,
	DECEPTIVE_TRAP
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
	#print(solution_vector)

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
	
# Sum function
func sum_genome(genome: PackedFloat32Array) -> float:
	var sum = 0.0
	for i in range(genome.size()):
		sum += genome[i];
	return sum

# Deceptive trap, increase score as more nonzero, but highest score is all zeroes
# Deceptive because the gradient points to a suboptimal local maximum, not the global optimum
func deceptive_trap(genome: PackedFloat32Array) -> float:
	var sum = 0.0
	var all_zero = true;
	for i in range(genome.size()):
		var zero = genome[i] < 0.5
		all_zero = all_zero and zero;
		if(not zero):
			sum += 1;
		
	return genome_size * genome_size if all_zero else sum

func _evaluate(genome: PackedFloat32Array) -> float:
	var fitness = 0.0
	if problem_type == ProblemType.ROSENBROCK:
		fitness = -rosenbrock(genome)
	elif problem_type == ProblemType.POLYNOMIAL:
		fitness = -polynomial(genome)
	elif problem_type == ProblemType.LEAST_SQUARES:
		fitness = -least_squares(genome)
	elif problem_type == ProblemType.SUM:
		fitness = sum_genome(genome)
	elif problem_type == ProblemType.DECEPTIVE_TRAP:
		fitness = deceptive_trap(genome)
	return fitness;

func _on_started(genome: PackedFloat32Array) -> void:
	self.ended.emit(_evaluate(genome))
