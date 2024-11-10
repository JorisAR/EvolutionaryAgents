import pandas as pd
import matplotlib.pyplot as plt
import sys

def plot_fitness(csv_path):
    # Read the CSV file
    data = pd.read_csv(csv_path)
    
    # Check if the required columns are present
    if 'generation' not in data.columns or 'mean_fitness' not in data.columns or 'best_fitness' not in data.columns:
        print("CSV file does not contain the required columns.")
        return
    
    # Plot the data
    plt.figure(figsize=(10, 6))
    plt.plot(data['generation'], data['mean_fitness'], label='Mean Fitness')
    plt.plot(data['generation'], data['best_fitness'], label='Best Fitness')
    
    # Add labels and title
    plt.xlabel('Generation')
    plt.ylabel('Fitness')
    plt.title('Fitness Over Generations')
    plt.legend()
    
    # Show the plot
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python fitness_plot.py <path_to_csv>")
    else:
        plot_fitness(sys.argv[1])
