#pragma once
#include "json.hpp"
#include <string>
#include <vector>
#include <map>

struct SynthPreset {
    std::string name;
    double oscillator1_freq;
    double oscillator2_freq;
    double filter_cutoff;
    double resonance;
    double attack;
    double decay;
    double sustain;
    double release;
};

class PresetManager {
public:
    PresetManager(const std::string& filename);
    
    // Create
    bool addPreset(const SynthPreset& preset);
    
    // Read
    std::vector<std::string> listPresets() const;
    SynthPreset getPreset(const std::string& name) const;
    
    // Update
    bool updatePreset(const std::string& name, const SynthPreset& preset);
    
    // Delete
    bool deletePreset(const std::string& name);
    
private:
    std::string filename_;
    std::map<std::string, JsonObject> presets_;
    
    void saveToFile();
    void loadFromFile();
    JsonObject presetToJson(const SynthPreset& preset) const;
    SynthPreset jsonToPreset(const JsonObject& j) const;
};
