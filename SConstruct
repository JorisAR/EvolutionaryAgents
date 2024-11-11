#!/usr/bin/env python
import os
import sys
# from scons_compiledb import compile_db # Import the compile_db function # Call the compile_db function to enable compile_commands.json generation 
# compile_db()


# Import the SConstruct from godot-cpp
env = SConscript("godot-cpp/SConstruct")

# Add necessary include directories
env.Append(CPPPATH=[    
    "src/neural_networks/",
    "src/evolutionary_algorithms/", 
    "src/evolutionary_algorithms/parameters/", 
    "src/evolutionary_algorithms/test_application/"  
    "src/gym/",   
    "src/utility/",   
    "src/", 
])

# Add main source files
sources = Glob("src/*.cpp") + Glob("src/utility/*.cpp")

evolutionary_sources = [
    "src/evolutionary_algorithms/evolutionary_algorithm.cpp",
    "src/evolutionary_algorithms/evolutionary_strategy.cpp",
    "src/evolutionary_algorithms/firefly_algorithm.cpp",
    "src/evolutionary_algorithms/sep_cma_es.cpp",
] + Glob("src/evolutionary_algorithms/parameters/*.cpp")


# Add neural network source files
neural_network_sources = [
    "src/neural_networks/neural_network.cpp"
]

gym_sources = [
    "src/gym/agent.cpp",
    "src/gym/evolutionary_gym.cpp"
]

sources = sources + evolutionary_sources + neural_network_sources + gym_sources

# Add test application source files
test_sources = Glob("src/evolutionary_algorithms/test_application/*.cpp")

# Handle different platforms
if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/libgdexample.{}.{}.framework/libgdexample.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "demo/bin/libgdexample.{}.{}.simulator.a".format(env["platform"], env["target"]),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            "demo/bin/libgdexample.{}.{}.a".format(env["platform"], env["target"]),
            source=sources,
        )
else:
    library = env.SharedLibrary(
        "demo/bin/libgdexample{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

# Build test application
test_program = env.Program(target='src/evolutionary_algorithms/test_application/test_program', source=sources+test_sources)

Default([library, test_program]) #])#
