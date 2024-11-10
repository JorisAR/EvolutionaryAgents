# GD Evolutionary Agents

This project is aimed at training neural networks using evolutionary algorithms within the Godot game engine. The project includes logging functionality to track the training progress and tools to visualize the results.

## Features

- **Neural Network**: A simple custom feed-forward neural network implementation.
- **Evolutionary Algorithms**: Implements various evolutionary strategies for training.
- **Logging**: Logs training data for later analysis.

## Getting Started

### Prerequisites

- [Godot Engine](https://godotengine.org/download)
- [Python](https://www.python.org/downloads/)
- Python libraries: `pandas`, `matplotlib`

### Installation

1. **Clone the repository**
   ```sh
   git clone https://github.com/your-repo/your-project.git
   cd your-project
   ```

2. **Install Python dependencies**
   ```sh
   pip install pandas matplotlib
   ```

3. **Open the project in Godot**
   - Launch Godot and open the project directory.

## Usage

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

- Inspired by various neural network and evolutionary algorithm implementations.
- Special thanks to the Godot community for their support and resources.
