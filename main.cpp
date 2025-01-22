#include "preset_manager.hpp"
#include "client_socket.hpp"
#include <iostream>
#include <limits>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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
    std::cout << "\nCreating new preset...\n";
    std::cout << "Enter preset name: ";
    std::cout.flush();  // Force output to display
    std::getline(std::cin, preset.name);
    
    std::cout << "Enter Oscillator 1 Frequency: ";
    std::cout.flush();
    std::cin >> preset.oscillator1_freq;
    
    std::cout << "Enter Oscillator 2 Frequency: ";
    std::cout.flush();
    std::cin >> preset.oscillator2_freq;
    
    std::cout << "Enter Filter Cutoff: ";
    std::cout.flush();
    std::cin >> preset.filter_cutoff;
    
    std::cout << "Enter Resonance (0-1): ";
    std::cout.flush();
    std::cin >> preset.resonance;
    
    std::cout << "Enter Attack time: ";
    std::cout.flush();
    std::cin >> preset.attack;
    
    std::cout << "Enter Decay time: ";
    std::cout.flush();
    std::cin >> preset.decay;
    
    std::cout << "Enter Sustain level (0-1): ";
    std::cout.flush();
    std::cin >> preset.sustain;
    
    std::cout << "Enter Release time: ";
    std::cout.flush();
    std::cin >> preset.release;
    
    clearInput();
    return preset;
}

void sendPresetToServer(const SynthPreset& preset) {
    std::cout << "\nAttempting to send preset to server...\n";
    ClientSocket client;
    if (!client.connect("localhost", 12345)) {
        std::cout << "Failed to connect to server. Make sure the server is running and 'Start Server' button is clicked in the GUI.\n";
        return;
    }
    std::cout << "Connected to server successfully\n";

    // Convert preset to JSON
    json j;
    j["name"] = preset.name;
    j["oscillator1_freq"] = preset.oscillator1_freq;
    j["oscillator2_freq"] = preset.oscillator2_freq;
    j["filter_cutoff"] = preset.filter_cutoff;
    j["resonance"] = preset.resonance;
    j["attack"] = preset.attack;
    j["decay"] = preset.decay;
    j["sustain"] = preset.sustain;
    j["release"] = preset.release;

    std::string json_str = j.dump();
    std::cout << "Sending JSON data: " << json_str << "\n";
    
    if (client.send_data(json_str)) {
        std::cout << "Data sent successfully, waiting for response...\n";
        std::string response = client.receive_data();
        std::cout << "Server response: " << response << "\n";
    } else {
        std::cout << "Failed to send data to server. The connection might have been lost.\n";
    }
    std::cout << "Finished sending preset to server\n\n";
}

int main() {
    PresetManager manager("synth_presets.json");
    ClientSocket client;

    while (true) {
        std::cout << "\nSynth Preset Manager\n";
        std::cout << "1. Create new preset\n";
        std::cout << "2. List all presets\n";
        std::cout << "3. Load preset\n";
        std::cout << "4. Delete preset\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter choice (1-5): ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInput();
            continue;
        }
        clearInput();

        if (choice < 1 || choice > 5) {
            std::cout << "Please enter a number between 1 and 5.\n";
            continue;
        }

        switch (choice) {
            case 1: {
                SynthPreset new_preset = getPresetFromUser();
                if (manager.addPreset(new_preset)) {
                    std::cout << "Preset added successfully!\n";
                    sendPresetToServer(new_preset);
                } else {
                    std::cout << "Failed to add preset. Name might already exist.\n";
                }
                break;
            }
            case 2: {
                std::cout << "\nAvailable presets:\n";
                for (const auto& name : manager.listPresets()) {
                    std::cout << "- " << name << "\n";
                }
                break;
            }
            case 3: {
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
                break;
            }
            case 4: {
                std::cout << "Enter preset name to delete: ";
                std::string name;
                std::getline(std::cin, name);
                if (manager.deletePreset(name)) {
                    std::cout << "Preset deleted successfully!\n";
                } else {
                    std::cout << "Failed to delete preset. Preset not found.\n";
                }
                break;
            }
            case 5:
                std::cout << "Exiting...\n";
                return 0;
            default:
                std::cout << "Invalid choice\n";
        }
    }
    
    return 0;
}

//g++ main.cpp preset_manager.cpp client_socket.cpp -o synth_presets
//./synth_presets
