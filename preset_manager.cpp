#include "preset_manager.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

PresetManager::PresetManager(const std::string& filename) : filename_(filename) {
    loadFromFile();
}

void PresetManager::loadFromFile() {
    try {
        std::ifstream file(filename_);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            
            // Remove the outer braces and split by preset
            size_t start = content.find('{', 1);
            while (start != std::string::npos) {
                size_t end = content.find('}', start);
                if (end == std::string::npos) break;
                
                std::string presetStr = content.substr(start, end - start + 1);
                JsonObject preset = JsonObject::fromString(presetStr);
                std::string name = preset.get("name");
                presets_[name] = preset;
                
                start = content.find('{', end + 1);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading presets: " << e.what() << std::endl;
    }
}

void PresetManager::saveToFile() {
    try {
        std::ofstream file(filename_);
        file << "{\n";
        bool first = true;
        for (const auto& [name, preset] : presets_) {
            if (!first) file << ",\n";
            file << "  \"" << name << "\": " << preset.toString();
            first = false;
        }
        file << "\n}\n";
    } catch (const std::exception& e) {
        std::cerr << "Error saving presets: " << e.what() << std::endl;
    }
}

JsonObject PresetManager::presetToJson(const SynthPreset& preset) const {
    JsonObject j;
    j.set("name", preset.name);
    j.set("oscillator1_freq", preset.oscillator1_freq);
    j.set("oscillator2_freq", preset.oscillator2_freq);
    j.set("filter_cutoff", preset.filter_cutoff);
    j.set("resonance", preset.resonance);
    j.set("attack", preset.attack);
    j.set("decay", preset.decay);
    j.set("sustain", preset.sustain);
    j.set("release", preset.release);
    return j;
}

SynthPreset PresetManager::jsonToPreset(const JsonObject& j) const {
    SynthPreset preset;
    preset.name = j.get("name");
    preset.oscillator1_freq = j.getDouble("oscillator1_freq");
    preset.oscillator2_freq = j.getDouble("oscillator2_freq");
    preset.filter_cutoff = j.getDouble("filter_cutoff");
    preset.resonance = j.getDouble("resonance");
    preset.attack = j.getDouble("attack");
    preset.decay = j.getDouble("decay");
    preset.sustain = j.getDouble("sustain");
    preset.release = j.getDouble("release");
    return preset;
}

bool PresetManager::addPreset(const SynthPreset& preset) {
    if (presets_.find(preset.name) != presets_.end()) {
        return false;
    }
    presets_[preset.name] = presetToJson(preset);
    saveToFile();
    return true;
}

std::vector<std::string> PresetManager::listPresets() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : presets_) {
        names.push_back(name);
    }
    return names;
}

SynthPreset PresetManager::getPreset(const std::string& name) const {
    auto it = presets_.find(name);
    if (it == presets_.end()) {
        throw std::runtime_error("Preset not found: " + name);
    }
    return jsonToPreset(it->second);
}

bool PresetManager::updatePreset(const std::string& name, const SynthPreset& preset) {
    if (presets_.find(name) == presets_.end()) {
        return false;
    }
    presets_[name] = presetToJson(preset);
    saveToFile();
    return true;
}

bool PresetManager::deletePreset(const std::string& name) {
    if (presets_.find(name) == presets_.end()) {
        return false;
    }
    presets_.erase(name);
    saveToFile();
    return true;
}
