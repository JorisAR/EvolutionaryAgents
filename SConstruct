#!/usr/bin/env python
import os
import sys
# from scons_compiledb import compile_db # Import the compile_db function # Call the compile_db function to enable compile_commands.json generation 
# compile_db()


# Import the SConstruct from godot-cpp
env = SConscript("godot-cpp/SConstruct")

# Add necessary include directories
env.Append(CPPPATH=[    
    "src/utility/",   
    "src/", 
    "src/neural_networks/",
    "src/evolutionary_algorithms/", 
    "src/evolutionary_algorithms/parameters/", 
    "src/evolutionary_algorithms/test_application/",
    "src/optimizer/sensors/",   
    "src/optimizer/",
])

# Add main source files
sources = Glob("src/*.cpp") + Glob("src/utility/*.cpp")

evolutionary_sources = [
    "src/evolutionary_algorithms/evolutionary_algorithm.cpp",
    "src/evolutionary_algorithms/evolutionary_strategy.cpp",
    "src/evolutionary_algorithms/genetic_algorithm.cpp",
    "src/evolutionary_algorithms/sep_cma_es.cpp",
    "src/evolutionary_algorithms/stochastic_ascent_ga.cpp",
] + Glob("src/evolutionary_algorithms/parameters/*.cpp")


# Add neural network source files
neural_network_sources = [
    "src/neural_networks/neural_network.cpp"
]

optimizer_sources = [
    "src/optimizer/evolutionary_optimizer.cpp",
    "src/optimizer/problem_optimizer.cpp",
    "src/optimizer/sensors/depth_sensor3d.cpp",
    "src/optimizer/agent.cpp",
    "src/optimizer/evolutionary_gym.cpp"    
]

sources = sources + evolutionary_sources + neural_network_sources + optimizer_sources

#documentation
if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# Handle different platforms
if env["platform"] == "macos":
    library = env.SharedLibrary(
        "project/addons/evolutionary_agents/bin/evolutionary_agents.{}.{}.framework/evolutionary_agents.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "project/addons/evolutionary_agents/bin/evolutionary_agents.{}.{}.simulator.a".format(env["platform"], env["target"]),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            "project/addons/evolutionary_agents/bin/evolutionary_agents.{}.{}.a".format(env["platform"], env["target"]),
            source=sources,
        )
else:
    library = env.SharedLibrary(
        "project/addons/evolutionary_agents/bin/evolutionary_agents{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )


Default(library)
