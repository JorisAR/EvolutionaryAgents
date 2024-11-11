#include <fstream>
#include <vector>
#include <string>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/project_settings.hpp>

class Logger {
public:
    Logger(const std::string &path) : file_path(path), generation_count(0) {
        std::string fs_path = get_filesystem_path(file_path);
        
        // Open the file in truncate mode to clear its contents if it already exists
        log_file.open(fs_path, std::ios_base::out | std::ios_base::trunc);
        if (!log_file.is_open()) {
            invalid = true;
            return;
        }
        // Write the header
        log_buffer.push_back("generation,mean_fitness,best_fitness");
    }

    ~Logger() {
        flush();
    }

    void log_generation(float mean_fitness, float best_fitness) {
        if(invalid) return;
        generation_count++;

        // Collect data
        log_buffer.push_back(std::to_string(generation_count) + "," + std::to_string(mean_fitness) + "," + std::to_string(best_fitness));
        
        // Write to file in bulk every 100 generations for performance
        if (log_buffer.size() >= 100) {
            write_to_file();
        }
    }

    void flush() {
        if(invalid) return;
        if (!log_buffer.empty()) {
            write_to_file();            
        }
        godot::UtilityFunctions::print(static_cast<godot::String>("Saved log to: " + godot::String(file_path.c_str())));
        if (log_file.is_open()) {
            log_file.close();
        }
        invalid = true;
    }

private:
    std::ofstream log_file;
    std::string file_path;
    std::vector<std::string> log_buffer;
    int generation_count;
    bool invalid = false;

    std::string get_filesystem_path(const std::string &resource_path) {
        if (resource_path.substr(0, 6) == "res://") {
            return godot::ProjectSettings::get_singleton()->globalize_path(godot::String(resource_path.c_str())).utf8().get_data();
        }
        return resource_path;
    }

    void write_to_file() {
        for (const auto &line : log_buffer) {
            log_file << line << "\n";
        }
        log_buffer.clear();
    }
};
