import matplotlib.pyplot as plt

# Read fitness values from the log file
generations = []
fitness_values = []

with open("fitness_log.txt", "r") as file:
    for line in file:
        values = list(map(float, line.strip().split()))
        fitness_values.append(values)
        generations.append(len(generations))

# Compute mean fitness for each generation
mean_fitness = [sum(gen) / len(gen) for gen in fitness_values]

# Plot the mean fitness over generations
plt.plot(generations, mean_fitness, label='Mean Fitness')
plt.xlabel('Generation')
plt.ylabel('Mean Fitness')
plt.title('Evolution of Mean Fitness over Generations')
plt.legend()
plt.show()
