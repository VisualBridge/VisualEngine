#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>
#include "entity.h"

class ProjectManager {
public:
    static ProjectManager& Instance() {
        static ProjectManager instance;
        return instance;
    }

    bool LoadProject(const std::string& path) {
        projectPath = path;
        projectData.clear();

        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open project file: " << path << std::endl;
            return false;
        }

        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
            line = trim(line);

            if (line.empty() || line[0] == '#') continue;

            size_t eqPos = line.find('=');
            if (eqPos == std::string::npos) {
                std::cerr << "Invalid syntax at line " << lineNum << ": " << line << std::endl;
                continue;
            }

            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));

            if (key.empty()) {
                std::cerr << "Empty key at line " << lineNum << std::endl;
                continue;
            }

            projectData[key] = value;
        }

        file.close();
        std::cout << "Project loaded: " << path << " (" << projectData.size() << " entries)" << std::endl;
        return true;
    }

    bool SaveProject(const std::string& path) {
        std::ofstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to save project file: " << path << std::endl;
            return false;
        }

        file << "# Visual Engine Project File\n";
        file << "# Syntax: key=value\n\n";

        for (const auto& [key, value] : projectData) {
            file << key << "=" << value << "\n";
        }

        file.close();
        projectPath = path;
        std::cout << "Project saved: " << path << std::endl;
        return true;
    }

    void SetValue(const std::string& key, const std::string& value) {
        projectData[key] = value;
    }

    std::string GetValue(const std::string& key, const std::string& defaultValue = "") const {
        auto it = projectData.find(key);
        return (it != projectData.end()) ? it->second : defaultValue;
    }

    bool HasKey(const std::string& key) const {
        return projectData.find(key) != projectData.end();
    }

    int GetInt(const std::string& key, int defaultValue = 0) const {
        auto it = projectData.find(key);
        if (it == projectData.end()) return defaultValue;
        try {
            return std::stoi(it->second);
        }
        catch (...) {
            return defaultValue;
        }
    }

    float GetFloat(const std::string& key, float defaultValue = 0.0f) const {
        auto it = projectData.find(key);
        if (it == projectData.end()) return defaultValue;
        try {
            return std::stof(it->second);
        }
        catch (...) {
            return defaultValue;
        }
    }

    bool GetBool(const std::string& key, bool defaultValue = false) const {
        auto it = projectData.find(key);
        if (it == projectData.end()) return defaultValue;
        std::string val = it->second;
        return (val == "true" || val == "1" || val == "yes");
    }

    const std::string& GetProjectPath() const { return projectPath; }
    const std::unordered_map<std::string, std::string>& GetAllData() const { return projectData; }

    void Clear() {
        projectData.clear();
        projectPath.clear();
    }

    bool CreateNewProject(const std::string& path, const std::string& projectName) {
        Clear();
        SetValue("project_name", projectName);
        SetValue("version", "1.0");
        SetValue("engine_version", "0.1.0");
        SetValue("scene_path", "scenes/main.scene");
        SetValue("assets_path", "assets/");
        SetValue("scripts_path", "scripts/");
        return SaveProject(path);
    }

private:
    ProjectManager() {}

    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }

    std::string projectPath;
    std::unordered_map<std::string, std::string> projectData;
};