#include "preset_manager.hpp"
#include <iostream>
#include <limits>

void printPreset(const SynthPreset& preset) {
    std::cout << "Name: " << preset.name << "\n"
              << "Oscillator 1 Frequency: " << preset.oscillator1_freq << "\n"
              << "Oscillator 2 Frequency: " << preset.oscillator2_freq << "\n"
              << "Filter Cutoff: " << preset.filter_cutoff << "\n"
              << "Resonance: " << preset.resonance << "\n"
              << "Attack: " << preset.attack << "\n"
              << "Decay: " << preset.decay << "\n"
              << "Sustain: " << preset.sustain << "\n"
              << "Release: " << preset.release << "\n";
}

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

SynthPreset getPresetFromUser() {
    SynthPreset preset;
    std::cout << "Enter preset name: ";
    std::getline(std::cin, preset.name);
    
    std::cout << "Enter Oscillator 1 Frequency: ";
    std::cin >> preset.oscillator1_freq;
    
    std::cout << "Enter Oscillator 2 Frequency: ";
    std::cin >> preset.oscillator2_freq;
    
    std::cout << "Enter Filter Cutoff: ";
    std::cin >> preset.filter_cutoff;
    
    std::cout << "Enter Resonance (0-1): ";
    std::cin >> preset.resonance;
    
    std::cout << "Enter Attack time: ";
    std::cin >> preset.attack;
    
    std::cout << "Enter Decay time: ";
    std::cin >> preset.decay;
    
    std::cout << "Enter Sustain level (0-1): ";
    std::cin >> preset.sustain;
    
    std::cout << "Enter Release time: ";
    std::cin >> preset.release;
    
    clearInput();
    return preset;
}

int main() {
    PresetManager manager("synth_presets.json");
    
    while (true) {
        std::cout << "\n=== Synth Preset Manager ===\n"
                  << "1. Add new preset\n"
                  << "2. List all presets\n"
                  << "3. View preset\n"
                  << "4. Update preset\n"
                  << "5. Delete preset\n"
                  << "6. Exit\n"
                  << "Choose an option (1-6): ";
        
        int choice;
        std::cin >> choice;
        clearInput();
        
        if (choice == 1) {
            auto preset = getPresetFromUser();
            if (manager.addPreset(preset)) {
                std::cout << "Preset added successfully!\n";
            } else {
                std::cout << "Failed to add preset. Name might already exist.\n";
            }
        }
        else if (choice == 2) {
            std::cout << "\nAvailable presets:\n";
            for (const auto& name : manager.listPresets()) {
                std::cout << "- " << name << "\n";
            }
        }
        else if (choice == 3) {
            std::cout << "Enter preset name to view: ";
            std::string name;
            std::getline(std::cin, name);
            try {
                auto preset = manager.getPreset(name);
                std::cout << "\nPreset details:\n";
                printPreset(preset);
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        }
        else if (choice == 4) {
            std::cout << "Enter preset name to update: ";
            std::string name;
            std::getline(std::cin, name);
            auto preset = getPresetFromUser();
            if (manager.updatePreset(name, preset)) {
                std::cout << "Preset updated successfully!\n";
            } else {
                std::cout << "Failed to update preset. Preset not found.\n";
            }
        }
        else if (choice == 5) {
            std::cout << "Enter preset name to delete: ";
            std::string name;
            std::getline(std::cin, name);
            if (manager.deletePreset(name)) {
                std::cout << "Preset deleted successfully!\n";
            } else {
                std::cout << "Failed to delete preset. Preset not found.\n";
            }
        }
        else if (choice == 6) {
            break;
        }
        else {
            std::cout << "Invalid option. Please try again.\n";
        }
    }
    
    return 0;
}

//g++ main.cpp preset_manager.cpp -o synth_presets
//./synth_presets
