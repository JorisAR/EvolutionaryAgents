# GD Evolutionary Agents

This project enables the use of evolutionary algorithms (EAs) within the Godot game engine. It's specifically designed for:
- Maximization problems, where one defines a function F(x), and the optimizer aims to find x such that F(x) is maximal.
- Reinforcement learning problems, where one defines a Gym, and an agent that obtains a fitness value over a run. The neural network acting as the state-action function is trained using EAs.
The project includes logging functionality to track the training progress and tools to visualize the results.

## Features

- **Neural Network**: A simple custom feed-forward neural network implementation.
- **Evolutionary Algorithms**: Implements various evolutionary strategies for training.
- **Logging**: Logs training data for later analysis.

## Getting Started

### Prerequisites

#### Core
- [Godot Engine](https://godotengine.org/download)

#### Visualisation
- [Python](https://www.python.org/downloads/)
- Python libraries: `pandas`, `matplotlib`
- Outputs are CSV, alternatives can be used.

### Installation

1. **Clone the repository**
   ```sh
   git clone https://github.com/JorisAR/GDEvolutionaryAgents
   cd GDEvolutionaryAgents
   ```

2. **Install Python dependencies**
   ```sh
   pip install pandas matplotlib
   ```

3. **Open the project in Godot**
   - Launch Godot and open the project directory.

## Usage
### Demos
- Please checkout the demos provided to learn how to structure the problems. To run them, just run the godot as a scene. The project is designed to work in editor, and training may not be reliable as a standalone application.

### Training

1. **Initialize Training**:
   - Configure the neural network and evolutionary algorithm parameters in Godot by creating resources.
   - Run the training by executing the main scene in Godot.

### Visualize Training Results

1. **Run the visualization script**:
   ```sh
   python plot_fitness.py path/to/your/log.csv
   ```

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by various neural network and evolutionary algorithm implementations and papers.
- Special thanks to the Godot community for their support and resources.
