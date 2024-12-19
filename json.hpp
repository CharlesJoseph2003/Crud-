#pragma once
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class JsonObject {
    std::map<std::string, std::string> values;
public:
    void set(const std::string& key, const std::string& value) {
        values[key] = value;
    }

    void set(const std::string& key, double value) {
        values[key] = std::to_string(value);
    }

    std::string get(const std::string& key) const {
        auto it = values.find(key);
        return it != values.end() ? it->second : "";
    }

    double getDouble(const std::string& key) const {
        auto it = values.find(key);
        return it != values.end() ? std::stod(it->second) : 0.0;
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "{\n";
        for (auto it = values.begin(); it != values.end(); ++it) {
            if (it != values.begin()) ss << ",\n";
            ss << "  \"" << it->first << "\": ";
            // Try to parse as number, if fails, treat as string
            try {
                std::stod(it->second);
                ss << it->second;
            } catch (...) {
                ss << "\"" << it->second << "\"";
            }
        }
        ss << "\n}";
        return ss.str();
    }

    static JsonObject fromString(const std::string& str) {
        JsonObject obj;
        size_t pos = 0;
        while ((pos = str.find("\"", pos)) != std::string::npos) {
            size_t keyEnd = str.find("\":", pos);
            if (keyEnd == std::string::npos) break;
            
            std::string key = str.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 2;
            
            while (pos < str.length() && std::isspace(str[pos])) pos++;
            
            bool isString = str[pos] == '\"';
            size_t valueStart = pos + (isString ? 1 : 0);
            size_t valueEnd = str.find(isString ? "\"" : ",", valueStart);
            if (valueEnd == std::string::npos) valueEnd = str.find("}", valueStart);
            if (valueEnd == std::string::npos) break;
            
            std::string value = str.substr(valueStart, valueEnd - valueStart);
            obj.set(key, value);
            
            pos = valueEnd + 1;
        }
        return obj;
    }
};
