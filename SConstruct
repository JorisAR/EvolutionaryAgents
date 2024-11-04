#!/usr/bin/env python
import os
import sys

# Import the SConstruct from godot-cpp
env = SConscript("godot-cpp/SConstruct")

# Add necessary include directories
env.Append(CPPPATH=[
    "src/", 
    "src/gym/", 
    "src/neural_networks/",
    "src/evolutionary_algorithms/", 
    "src/evolutionary_algorithms/test_application/"    
])

# Add main source files
sources = Glob("src/*.cpp") + \
          Glob("src/gym/*.cpp")

# Manually specify the order for evolutionary_algorithms
evolutionary_sources = [
    "src/evolutionary_algorithms/evolutionary_algorithm.cpp",
    "src/evolutionary_algorithms/evolutionary_strategy.cpp"
]

# Add neural network source files
neural_network_sources = [
    "src/neural_networks/neural_network.cpp"
]

# Add test application source files
test_sources = Glob("src/evolutionary_algorithms/test_application/*.cpp")

# Handle different platforms
if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/libgdexample.{}.{}.framework/libgdexample.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources + evolutionary_sources + neural_network_sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "demo/bin/libgdexample.{}.{}.simulator.a".format(env["platform"], env["target"]),
            source=sources + evolutionary_sources + neural_network_sources,
        )
    else:
        library = env.StaticLibrary(
            "demo/bin/libgdexample.{}.{}.a".format(env["platform"], env["target"]),
            source=sources + evolutionary_sources + neural_network_sources,
        )
else:
    library = env.SharedLibrary(
        "demo/bin/libgdexample{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources + evolutionary_sources + neural_network_sources,
    )

# Build test application
test_program = env.Program(target='src/evolutionary_algorithms/test_application/test_program', source=test_sources + evolutionary_sources + neural_network_sources)

Default([library, test_program])
